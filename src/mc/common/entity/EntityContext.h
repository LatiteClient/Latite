#pragma once

#include "EntityIdTraits.h"
#include "IEntityComponent.h"

template<>
struct entt::entt_traits<EntityId> : entt::basic_entt_traits<EntityIdTraits> {
    static constexpr std::size_t page_size = 2048;
};

template<std::derived_from<IEntityComponent> Type>
struct entt::component_traits<Type, EntityId> {
    using element_type = Type;
    using entity_type = EntityId;
    static constexpr bool in_place_delete = true;
    static constexpr std::size_t page_size = 128 * !std::is_empty_v<Type>;
};

template<typename Type>
struct entt::storage_type<Type, EntityId> {
    using type = basic_storage<Type, EntityId>;
};

template<std::derived_from<IEntityComponent> Type>
struct entt::type_hash<Type> {
    [[nodiscard]] static consteval id_type value() noexcept {
        constexpr auto hash = Type::type_hash; // Pre-define hash to avoid compiler shenanigans
        return hash;
    }

    [[nodiscard]] consteval operator id_type() const noexcept { return value(); }
};

class EntityRegistry : public std::enable_shared_from_this<EntityRegistry> {
public:
    std::string name;
    entt::basic_registry<EntityId> ownedRegistry;
    uint32_t id;

    template<typename T>
    T* tryGetGlobalComponent() {
        return this->ownedRegistry.ctx().find<T>();
    }
};

class EntityContext {
public:
    EntityRegistry& registry;
    entt::basic_registry<EntityId>& enttRegistry;
    EntityId entity;
};
