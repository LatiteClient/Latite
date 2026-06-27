#pragma once
#include "../../Module.h"
#include "client/misc/DiscordIpcClient.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

class ChatMessageEvent;
class PacketReceiveEvent;
class SendPacketEvent;
class UpdateEvent;

class DiscordPresence final : public Module {
public:
    DiscordPresence();
    ~DiscordPresence() override;

    void onEnable() override;
    void onDisable() override;

private:
    struct ServerPresence {
        std::string_view address;
        std::string_view featuredServer;
        std::string_view name;
        std::string_view logoKey;
        std::string_view logoTooltip;
        bool tracksHiveGame = false;
    };

    const std::array<ServerPresence, 5> knownServers = {
        ServerPresence { "hivebedrock.network", "The Hive", "The Hive", "thehive", "The Hive Logo", true },
        ServerPresence { "cubecraft.net", {}, "CubeCraft", "cubecraft", "CubeCraft Games Logo" },
        ServerPresence { "play.galaxite.net", {}, "Galaxite", "galaxite", "Galaxite Network Logo" },
        ServerPresence { "zeqa.net", {}, "Zeqa", "zeqa", "Zeqa Practice Logo" },
        ServerPresence { "nethergames.org", {}, "NetherGames", "nethergames", "NetherGames Network Logo" },
    };

    const std::array<std::pair<std::string_view, std::string_view>, 14> hiveGameNames = {
        std::pair<std::string_view, std::string_view> { "WARS", "Treasure Wars" },
        std::pair<std::string_view, std::string_view> { "DR", "DeathRun" },
        std::pair<std::string_view, std::string_view> { "HIDE", "Hide and Seek" },
        std::pair<std::string_view, std::string_view> { "SG", "Survival Games" },
        std::pair<std::string_view, std::string_view> { "MURDER", "Murder Mystery" },
        std::pair<std::string_view, std::string_view> { "SKY", "SkyWars" },
        std::pair<std::string_view, std::string_view> { "CTF", "Capture the Flag" },
        std::pair<std::string_view, std::string_view> { "DROP", "Block Drop" },
        std::pair<std::string_view, std::string_view> { "GROUND", "Ground Wars" },
        std::pair<std::string_view, std::string_view> { "BUILD", "Build Battle" },
        std::pair<std::string_view, std::string_view> { "PARTY", "Block Party" },
        std::pair<std::string_view, std::string_view> { "BRIDGE", "The Bridge" },
        std::pair<std::string_view, std::string_view> { "GRAV", "Gravity" },
        std::pair<std::string_view, std::string_view> { "BED", "BedWars" },
    };

    const std::string discordApplicationId = "1066896173799047199";
    const std::chrono::seconds presenceCheckInterval { 5 };
    const std::chrono::seconds presenceRefreshInterval { 60 };
    const std::chrono::seconds hiveConnectionRefreshDelay { 3 };
    const std::chrono::seconds hiveConnectionResponseWindow { 20 };

    void onUpdate(UpdateEvent& ev);
    void onPacketReceive(PacketReceiveEvent& ev);
    void onChatMessage(ChatMessageEvent& ev);
    void onSendPacket(SendPacketEvent& ev);
    void updateConnectionState();
    void publishPresence(bool force);
    DiscordIpcClient::Activity makeActivity() const;

    std::optional<DiscordIpcClient> ipcClient;
    std::optional<DiscordIpcClient::Activity> lastSentActivity;
    std::optional<std::string> hiveGameModeCode;
    ServerPresence const* activeServer = nullptr;
    std::string activeServerAddress;
    std::chrono::steady_clock::time_point lastCheck {};
    std::chrono::steady_clock::time_point lastRefresh {};
    std::chrono::steady_clock::time_point connectionRefreshAt {};
    std::chrono::steady_clock::time_point suppressConnectionResponsesUntil {};
    std::int64_t sessionStart = 0;
    bool sendingConnectionRequest = false;
};
