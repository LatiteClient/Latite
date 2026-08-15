#pragma once

#include <span>
#include <string_view>

namespace SDK::Social {
    class GameConnectionInfo;
}

namespace ServerDetection {
    enum class ServerId {
        Hive,
        CubeCraft,
        Galaxite,
        Zeqa,
    };

    struct ServerDefinition {
        ServerId id;
        std::string_view name;
        std::span<const std::string_view> addresses;
        std::span<const std::string_view> featuredServerNames;
    };

    [[nodiscard]] std::span<const ServerDefinition> getDefinitions() noexcept;
    [[nodiscard]] const ServerDefinition& getDefinition(ServerId id) noexcept;
    [[nodiscard]] const ServerDefinition* identify(SDK::Social::GameConnectionInfo* connectionInfo) noexcept;
    [[nodiscard]] bool matches(SDK::Social::GameConnectionInfo* connectionInfo, ServerId id) noexcept;
}
