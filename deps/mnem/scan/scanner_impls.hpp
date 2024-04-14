#pragma once
#include <mnemosyne/scan/scanner.hpp>

namespace mnem::internal {
    // Auxilary functions for implementations
    template <size_t Align, class T>
    T* align_ptr(T* ptr) {
        static constexpr auto ALIGN_MASK = static_cast<uintptr_t>(Align - 1);
        return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(ptr) & ~ALIGN_MASK);
    }

    template <size_t Align, class T>
    T* align_ptr_up(T* ptr) {
        static constexpr auto ALIGN_MASK = static_cast<uintptr_t>(Align - 1);
        return reinterpret_cast<T*>((reinterpret_cast<uintptr_t>(ptr) + ALIGN_MASK) & ~ALIGN_MASK);
    }

    const std::byte* scan_impl_normal(const std::byte* begin, const std::byte* end, signature sig);
    const std::byte* scan_impl_avx2(const std::byte* begin, const std::byte* end, signature sig);
}
