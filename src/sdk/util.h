#pragma once
#include <cstdint>
#include "Version.h"

namespace util {
    template <typename TreatAs, typename Pointer>
    extern TreatAs& directAccess(Pointer ptr, size_t offset) {
        return *reinterpret_cast<TreatAs*>(reinterpret_cast<uintptr_t>(ptr) + offset);
    }
}

#define CLASS_FIELD(type, name, offset)                                                                      \
    __declspec(property(get = __get_field_##name, put = __set_field_##name)) type name;                             \
    type &__get_field_##name() const { return util::directAccess<type>(this, offset); }                                    \
    template<typename T> void __set_field_##name(const T &value) { util::directAccess<type>(this, offset) = value; }

#define MV_DETAIL_GETOFFSET(offs1, offs2, offs3) \
    sdk::internalVers == sdk::VLATEST ? offs1 : \
    sdk::internalVers == sdk::V1_19_51 ? offs3 : offs2

#define MVCLASS_FIELD(type, name, offset1, offset2, offset3) __declspec(property(get = __get_field_##name, put = __set_field_##name)) type name;                             \
type& __get_field_##name() const { return util::directAccess<type>(this, MV_DETAIL_GETOFFSET(offset1, offset2, offset3)); }                                    \
template<typename T> void __set_field_##name(const T& value) { util::directAccess<type>(this, (MV_DETAIL_GETOFFSET(offset1, offset2, offset3))) = value; }
