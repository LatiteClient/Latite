#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Psapi.h>
#include <dbghelp.h>

#include <algorithm>

#include <mnemosyne/mem/process.hpp>

auto mnem::proc_module::get_memory_range() const -> memory_span {
    auto mod = reinterpret_cast<HMODULE>(this->ptr);

    MODULEINFO info{};
    GetModuleInformation(GetCurrentProcess(), mod, &info, sizeof(info));

    return memory_span{ reinterpret_cast<std::byte*>(info.lpBaseOfDll), info.SizeOfImage };
}

auto mnem::proc_module::get_section_range(std::string_view name) const -> std::optional<memory_span> {
    IMAGE_NT_HEADERS* ntHeaders = ImageNtHeader(this->ptr);

    auto sectionHeader = reinterpret_cast<IMAGE_SECTION_HEADER*>(ntHeaders + 1);

    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; sectionHeader++, i++) {
        auto sectionName = reinterpret_cast<const char*>(sectionHeader->Name);
        if (strncmp(name.data(), sectionName, std::min(name.size(), {8})) == 0) {
            return memory_span{ reinterpret_cast<std::byte*>(this->ptr) + sectionHeader->VirtualAddress, sectionHeader->Misc.VirtualSize };
        }
    }

    return {};
}

auto mnem::get_main_proc_module() -> proc_module {
    return { GetModuleHandleA(nullptr) };
}

auto mnem::get_proc_module(const std::string& name) -> std::optional<proc_module> {
    auto handle = GetModuleHandleA(name.c_str());
    return handle ? std::make_optional(proc_module{ handle }) : std::nullopt;
}
