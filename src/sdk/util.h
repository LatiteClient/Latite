#pragma once
#include <cstdint>
#include <functional>
#include "Version.h"
#include "Util/Meta.h"

namespace SDK {
    namespace detail {
    }

    template <typename ... PossibleTypes>
    class Unknown {
    public:
        template <typename AsType>
        [[nodiscard]] inline AsType* _as() noexcept {
            return reinterpret_cast<AsType*>(this);
        }

        // consistent with MVCLASS_FIELD: 1. latest, 2. 1.18.12, 3: 1.19.51
        [[nodiscard]] inline void _do(std::function<void(Unknown<PossibleTypes...>)> func) noexcept {

        }
    };
}

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
    SDK::internalVers == SDK::VLATEST ? offs1 : \
    SDK::internalVers == SDK::V1_19_51 ? offs3 : offs2

#define MVCLASS_FIELD(type, name, offset1, offset2, offset3) __declspec(property(get = __get_field_##name, put = __set_field_##name)) type name;                             \
type& __get_field_##name() const { return util::directAccess<type>(this, MV_DETAIL_GETOFFSET(offset1, offset2, offset3)); }                                    \
template<typename T> void __set_field_##name(const T& value) { util::directAccess<type>(this, (MV_DETAIL_GETOFFSET(offset1, offset2, offset3))) = value; }
