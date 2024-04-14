#pragma once
#include "../../../defines.hpp"

#ifdef MNEMOSYNE_ABI_ITANIUM

#include <bit>
#include <cstdint>
#include <cstddef>

namespace mnem::internal {
    template <class F, class T>
    inline size_t vfunc_index_impl(F T::*fn) {
        // https://itanium-cxx-abi.github.io/cxx-abi/abi.html#member-function-pointers
        struct memfn {
            uintptr_t ptr;
            ptrdiff_t adj;
        } func = std::bit_cast<memfn>(fn);
        // TODO: if (!(ptr & 1)) throw std::runtime_error("member function must be virtual");
        return func.ptr / sizeof(uintptr_t);
    }
}

#endif