#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

#include "GameAllocator.h"

namespace {
    // Bedrock::Memory::IMemoryAllocator
    struct IMemoryAllocator {
        virtual ~IMemoryAllocator() = default;
        virtual void* allocate(size_t size) = 0;
        virtual void release(void* ptr) = 0;
        virtual void* alignedAllocate(size_t size, size_t alignment) = 0;
        virtual void alignedRelease(void* ptr) = 0;
        virtual size_t getUsableSize(void* ptr, bool aligned) = 0;
        virtual void* _realloc(void* ptr, size_t size) = 0;
        virtual void* _alignedRealloc(void* ptr, size_t size, size_t alignment) = 0;
    };

    // push rsi; sub rsp, 20h; mov rsi, rcx
    // mov rcx, [rip+gAllocator] <- +8, disp32 at +0xB, next instruction at +0xF
    // mov rax, [rcx]
    constexpr int16_t operatorNewSig[] = {
        0x56, 0x48, 0x83, 0xEC, -1, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x0D, -1, -1, -1, -1, 0x48, 0x8B, 0x01,
    };
    constexpr size_t allocatorRefOffset = 8;

    enum class Backend {
        Game,
        MallocRedirect,
        Crt
    };

    struct State {
        Backend backend = Backend::Crt;
        IMemoryAllocator** gameAllocator = nullptr;
        decltype(&std::malloc) rMalloc = nullptr;
        decltype(&std::free) rFree = nullptr;
        decltype(&_aligned_malloc) rAlignedMalloc = nullptr;
        decltype(&_aligned_free) rAlignedFree = nullptr;
    };

    // Plain byte scan over the executable sections of the game module. Must not allocate.
    uintptr_t findOperatorNew(HMODULE module) {
        const auto base = reinterpret_cast<uintptr_t>(module);
        const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return 0;
        const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE) return 0;

        constexpr size_t sigLen = sizeof(operatorNewSig) / sizeof(operatorNewSig[0]);
        const auto* section = IMAGE_FIRST_SECTION(nt);
        for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++, section++) {
            if (!(section->Characteristics & IMAGE_SCN_MEM_EXECUTE)) continue;

            const auto* begin = reinterpret_cast<const uint8_t*>(base + section->VirtualAddress);
            const size_t size = section->Misc.VirtualSize;
            if (size < sigLen) continue;
            const auto* last = begin + size - sigLen;

            for (const auto* p = begin; p <= last; p++) {
                p = static_cast<const uint8_t*>(std::memchr(p, operatorNewSig[0], static_cast<size_t>(last - p) + 1));
                if (!p) break;

                bool match = true;
                for (size_t j = 1; j < sigLen; j++) {
                    if (operatorNewSig[j] >= 0 && p[j] != static_cast<uint8_t>(operatorNewSig[j])) {
                        match = false;
                        break;
                    }
                }
                if (match) return reinterpret_cast<uintptr_t>(p);
            }
        }
        return 0;
    }

    State resolve() {
        State s {};

        if (const auto op = findOperatorNew(GetModuleHandleW(nullptr))) {
            const auto ref = op + allocatorRefOffset;
            const auto disp = *reinterpret_cast<const int32_t*>(ref + 3);
            const auto global = reinterpret_cast<IMemoryAllocator**>(ref + 7 + disp);
            if (*global) {
                s.backend = Backend::Game;
                s.gameAllocator = global;
                return s;
            }
        }

        // Signature broke (ex. a new game version): MallocRedirect is the heap the game's
        // allocator sits on, so it is still compatible for plain allocations.
        if (const auto mr = GetModuleHandleW(L"MallocRedirect.dll")) {
            s.rMalloc = reinterpret_cast<decltype(s.rMalloc)>(GetProcAddress(mr, "malloc"));
            s.rFree = reinterpret_cast<decltype(s.rFree)>(GetProcAddress(mr, "free"));
            s.rAlignedMalloc = reinterpret_cast<decltype(s.rAlignedMalloc)>(GetProcAddress(mr, "_aligned_malloc"));
            s.rAlignedFree = reinterpret_cast<decltype(s.rAlignedFree)>(GetProcAddress(mr, "_aligned_free"));
            if (s.rMalloc && s.rFree && s.rAlignedMalloc && s.rAlignedFree) {
                s.backend = Backend::MallocRedirect;
                return s;
            }
        }

        // Not running in a game that uses a custom allocator.
        return State {};
    }

    const State& state() {
        static const State s = resolve();
        return s;
    }

    void* allocRaw(size_t size) noexcept {
        const auto& s = state();
        switch (s.backend) {
        case Backend::Game:
            return (*s.gameAllocator)->allocate(size);
        case Backend::MallocRedirect:
            return s.rMalloc(size ? size : 1);
        default:
            return std::malloc(size ? size : 1);
        }
    }

    void freeRaw(void* ptr) noexcept {
        if (!ptr) return;
        const auto& s = state();
        switch (s.backend) {
        case Backend::Game:
            (*s.gameAllocator)->release(ptr);
            break;
        case Backend::MallocRedirect:
            s.rFree(ptr);
            break;
        default:
            std::free(ptr);
            break;
        }
    }

    void* alignedAllocRaw(size_t size, std::align_val_t align) noexcept {
        const auto& s = state();
        const auto alignment = static_cast<size_t>(align);
        switch (s.backend) {
        case Backend::Game:
            return (*s.gameAllocator)->alignedAllocate(size, alignment);
        case Backend::MallocRedirect:
            return s.rAlignedMalloc(size ? size : 1, alignment);
        default:
            return _aligned_malloc(size ? size : 1, alignment);
        }
    }

    void alignedFreeRaw(void* ptr) noexcept {
        if (!ptr) return;
        const auto& s = state();
        switch (s.backend) {
        case Backend::Game:
            (*s.gameAllocator)->alignedRelease(ptr);
            break;
        case Backend::MallocRedirect:
            s.rAlignedFree(ptr);
            break;
        default:
            _aligned_free(ptr);
            break;
        }
    }

    void* allocOrThrow(size_t size) {
        if (auto* p = allocRaw(size)) return p;
        throw std::bad_alloc {};
    }

    void* alignedAllocOrThrow(size_t size, std::align_val_t align) {
        if (auto* p = alignedAllocRaw(size, align)) return p;
        throw std::bad_alloc {};
    }
}

const char* GameAllocator::backendName() {
    switch (state().backend) {
    case Backend::Game:
        return "game";
    case Backend::MallocRedirect:
        return "MallocRedirect";
    default:
        return "crt";
    }
}

void* operator new(size_t size) {
    return allocOrThrow(size);
}
void* operator new[](size_t size) {
    return allocOrThrow(size);
}
void* operator new(size_t size, const std::nothrow_t&) noexcept {
    return allocRaw(size);
}
void* operator new[](size_t size, const std::nothrow_t&) noexcept {
    return allocRaw(size);
}

void operator delete(void* ptr) noexcept {
    freeRaw(ptr);
}
void operator delete[](void* ptr) noexcept {
    freeRaw(ptr);
}
void operator delete(void* ptr, size_t) noexcept {
    freeRaw(ptr);
}
void operator delete[](void* ptr, size_t) noexcept {
    freeRaw(ptr);
}
void operator delete(void* ptr, const std::nothrow_t&) noexcept {
    freeRaw(ptr);
}
void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
    freeRaw(ptr);
}

void* operator new(size_t size, std::align_val_t align) {
    return alignedAllocOrThrow(size, align);
}
void* operator new[](size_t size, std::align_val_t align) {
    return alignedAllocOrThrow(size, align);
}
void* operator new(size_t size, std::align_val_t align, const std::nothrow_t&) noexcept {
    return alignedAllocRaw(size, align);
}
void* operator new[](size_t size, std::align_val_t align, const std::nothrow_t&) noexcept {
    return alignedAllocRaw(size, align);
}

void operator delete(void* ptr, std::align_val_t) noexcept {
    alignedFreeRaw(ptr);
}
void operator delete[](void* ptr, std::align_val_t) noexcept {
    alignedFreeRaw(ptr);
}
void operator delete(void* ptr, size_t, std::align_val_t) noexcept {
    alignedFreeRaw(ptr);
}
void operator delete[](void* ptr, size_t, std::align_val_t) noexcept {
    alignedFreeRaw(ptr);
}
void operator delete(void* ptr, std::align_val_t, const std::nothrow_t&) noexcept {
    alignedFreeRaw(ptr);
}
void operator delete[](void* ptr, std::align_val_t, const std::nothrow_t&) noexcept {
    alignedFreeRaw(ptr);
}
