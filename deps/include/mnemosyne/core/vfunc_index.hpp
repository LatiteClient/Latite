#pragma once
#include "../defines.hpp"

#ifdef MNEMOSYNE_ABI_MICROSOFT
#include "../internal/abi/microsoft/vfunc_index_impl.hpp"
#else
#include "../internal/abi/itanium/vfunc_index_impl.hpp"
#endif

namespace mnem {
    // TODO: Deprecate this. It's extremely unsafe, and it's only here because the original proprietary library had it and Chronos needs it.
    template <class F, class T>
    inline size_t vfunc_index(F T::* fn) {
        return internal::vfunc_index_impl(fn);
    }
}