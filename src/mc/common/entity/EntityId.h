#pragma once

struct EntityId {
    uint32_t rawId;

    [[nodiscard]] constexpr bool operator==(const EntityId& other) const = default;

    [[nodiscard]] constexpr operator uint32_t() const { return this->rawId; }
};
