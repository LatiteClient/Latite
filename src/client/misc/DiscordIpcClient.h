#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include <Windows.h>
#include <nlohmann/json.hpp>

class DiscordIpcClient final {
public:
    struct Activity {
        std::string details;
        std::string state;
        std::string largeImageKey = "latite";
        std::string largeImageText = "Latite Client Logo";
        std::string smallImageKey;
        std::string smallImageText;
        std::int64_t startTimestamp = 0;

        bool operator==(const Activity&) const = default;
    };

    explicit DiscordIpcClient(std::string applicationId);
    ~DiscordIpcClient();

    DiscordIpcClient(const DiscordIpcClient&) = delete;
    DiscordIpcClient& operator=(const DiscordIpcClient&) = delete;
    DiscordIpcClient(DiscordIpcClient&&) = delete;
    DiscordIpcClient& operator=(DiscordIpcClient&&) = delete;

    bool setActivity(const Activity& activity);
    void clearActivity();
    void disconnect();

    [[nodiscard]] bool isConnected() const noexcept;

private:
    enum class Opcode : std::uint32_t {
        Handshake = 0,
        Frame = 1,
        Close = 2,
        Ping = 3,
        Pong = 4
    };

    bool ensureConnected();
    bool connect();
    bool openPipe(int pipeIndex);
    bool sendHandshake();
    bool sendSetActivity(const nlohmann::json& activity);
    bool sendFrame(Opcode opcode, const nlohmann::json& payload);
    bool writeAll(const void* data, std::size_t size) const;
    bool drainIncoming();
    void closePipe();
    void scheduleReconnect();

    std::string applicationId;
    HANDLE pipe = INVALID_HANDLE_VALUE;
    std::vector<std::uint8_t> readBuffer;
    std::uint64_t nonce = 0;
    std::chrono::steady_clock::time_point nextConnectAttempt {};
};
