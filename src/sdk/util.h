#pragma once
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
