#include "pch.h"
#include "DiscordPresence.h"

#include "client/event/events/ChatMessageEvent.h"
#include "client/event/events/PacketReceiveEvent.h"
#include "client/event/events/SendPacketEvent.h"
#include "client/event/events/UpdateEvent.h"
#include "mc/common/network/MinecraftPackets.h"
#include "mc/common/network/packet/CommandRequestPacket.h"
#include "mc/common/network/RakNetConnector.h"

#include <cctype>

DiscordPresence::DiscordPresence()
    : Module("DiscordPresence", LocalizeString::get("client.module.discordPresence.name"),
             LocalizeString::get("client.module.discordPresence.desc"), GAME) {
    Eventing::get().listen<UpdateEvent, &DiscordPresence::onUpdate>(this);
    Eventing::get().listen<PacketReceiveEvent, &DiscordPresence::onPacketReceive>(this);
    Eventing::get().listen<ChatMessageEvent, &DiscordPresence::onChatMessage>(this, 10);
    Eventing::get().listen<SendPacketEvent, &DiscordPresence::onSendPacket>(this, 10);
}

DiscordPresence::~DiscordPresence() {
    onDisable();
}

void DiscordPresence::onEnable() {
    if (sessionStart <= 0) {
        sessionStart = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    lastCheck = {};
    lastRefresh = {};
    connectionRefreshAt = {};
    suppressConnectionResponsesUntil = {};
    sendingConnectionRequest = false;
    lastSentActivity.reset();
    hiveGameModeCode.reset();
    activeServer = nullptr;
    activeServerAddress.clear();
    ipcClient.emplace(discordApplicationId);

    updateConnectionState();
    publishPresence(true);
}

void DiscordPresence::onDisable() {
    if (ipcClient) {
        ipcClient->clearActivity();
        ipcClient.reset();
    }

    lastSentActivity.reset();
    hiveGameModeCode.reset();
    activeServer = nullptr;
    activeServerAddress.clear();
    connectionRefreshAt = {};
    suppressConnectionResponsesUntil = {};
    sendingConnectionRequest = false;
}

void DiscordPresence::onUpdate(UpdateEvent&) {
    updateConnectionState();
    publishPresence(false);
}

void DiscordPresence::onPacketReceive(PacketReceiveEvent& ev) {
    const SDK::PacketID packetId = ev.getPacket()->getID();
    if (packetId != SDK::PacketID::CHANGE_DIMENSION && packetId != SDK::PacketID::TRANSFER) {
        return;
    }

    hiveGameModeCode.reset();
    suppressConnectionResponsesUntil = {};
    connectionRefreshAt = activeServer && activeServer->tracksHiveGame
                              ? std::chrono::steady_clock::now() + hiveConnectionRefreshDelay
                              : std::chrono::steady_clock::time_point {};
    lastSentActivity.reset();
    lastCheck = {};
}

void DiscordPresence::onChatMessage(ChatMessageEvent& ev) {
    std::string message;
    const std::string rawMessage = ev.getMessage();
    message.reserve(rawMessage.size());

    for (size_t i = 0; i < rawMessage.size(); i++) {
        const auto ch = static_cast<unsigned char>(rawMessage[i]);
        if (ch == 0xC2 && i + 2 < rawMessage.size() && static_cast<unsigned char>(rawMessage[i + 1]) == 0xA7) {
            i += 2;
            continue;
        }
        if (ch == 0xA7 && i + 1 < rawMessage.size()) {
            i++;
            continue;
        }
        message.push_back(rawMessage[i]);
    }

    while (!message.empty() && std::isspace(static_cast<unsigned char>(message.front()))) {
        message.erase(message.begin());
    }
    while (!message.empty() && std::isspace(static_cast<unsigned char>(message.back()))) {
        message.pop_back();
    }

    constexpr std::string_view serverNamePrefix = "You are connected to server name ";
    const bool isConnectionMessage = message.starts_with("You are connected to internal IP ") ||
                                     message.starts_with("You are connected to public IP ") ||
                                     message.starts_with(serverNamePrefix) ||
                                     message.starts_with("You are connected to server ");
    if (!isConnectionMessage) {
        return;
    }

    if (message.starts_with(serverNamePrefix)) {
        std::string gameCode = message.substr(serverNamePrefix.size());
        while (!gameCode.empty() && std::isspace(static_cast<unsigned char>(gameCode.back()))) {
            gameCode.pop_back();
        }
        while (!gameCode.empty() && std::isdigit(static_cast<unsigned char>(gameCode.back()))) {
            gameCode.pop_back();
        }
        while (!gameCode.empty() && std::isspace(static_cast<unsigned char>(gameCode.back()))) {
            gameCode.pop_back();
        }

        if (!gameCode.empty() && hiveGameModeCode != gameCode) {
            hiveGameModeCode = std::move(gameCode);
            lastSentActivity.reset();
            lastCheck = {};
        }
    }

    if (std::chrono::steady_clock::now() <= suppressConnectionResponsesUntil) {
        ev.setCancelled();
    }
}

void DiscordPresence::onSendPacket(SendPacketEvent& ev) {
    if (ev.getPacket()->getID() != SDK::PacketID::COMMAND_REQUEST) {
        return;
    }

    const auto* command = reinterpret_cast<SDK::CommandRequestPacket*>(ev.getPacket());
    if (util::ToLower(command->command) != "/connection" || sendingConnectionRequest) {
        return;
    }

    connectionRefreshAt = {};
    suppressConnectionResponsesUntil = {};
}

void DiscordPresence::updateConnectionState() {
    const auto now = std::chrono::steady_clock::now();
    SDK::ClientInstance* clientInstance = SDK::ClientInstance::get();
    SDK::RakNetConnector* connector = SDK::RakNetConnector::get();

    std::string serverAddress;
    if (clientInstance && clientInstance->minecraft && clientInstance->minecraft->getLevel() &&
        clientInstance->getLocalPlayer() && connector) {
        serverAddress = connector->dns + '\n' + connector->ipAddress + '\n' + connector->featuredServer;
    }

    if (serverAddress != activeServerAddress) {
        activeServerAddress = std::move(serverAddress);
        activeServer = nullptr;
        for (const ServerPresence& server : knownServers) {
            if (connector && (connector->dns.find(server.address) != std::string::npos ||
                              connector->ipAddress.find(server.address) != std::string::npos ||
                              (!server.featuredServer.empty() && connector->featuredServer == server.featuredServer))) {
                activeServer = &server;
                break;
            }
        }

        hiveGameModeCode.reset();
        suppressConnectionResponsesUntil = {};
        connectionRefreshAt = activeServer && activeServer->tracksHiveGame ? now + hiveConnectionRefreshDelay
                                                                           : std::chrono::steady_clock::time_point {};
        lastSentActivity.reset();
        lastCheck = {};
    }

    if (connectionRefreshAt == std::chrono::steady_clock::time_point {} || now < connectionRefreshAt || !activeServer ||
        !activeServer->tracksHiveGame || !clientInstance || !clientInstance->getLocalPlayer() ||
        !clientInstance->getLocalPlayer()->packetSender) {
        return;
    }

    std::shared_ptr<SDK::Packet> packet = SDK::MinecraftPackets::createPacket(SDK::PacketID::COMMAND_REQUEST);
    if (!packet) {
        return;
    }

    auto* command = reinterpret_cast<SDK::CommandRequestPacket*>(packet.get());
    command->applyCommand("/connection");

    connectionRefreshAt = {};
    suppressConnectionResponsesUntil = now + hiveConnectionResponseWindow;
    sendingConnectionRequest = true;
    clientInstance->getLocalPlayer()->packetSender->sendToServer(packet.get());
    sendingConnectionRequest = false;
}

void DiscordPresence::publishPresence(bool force) {
    if (!ipcClient) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    if (!force && now - lastCheck < presenceCheckInterval) {
        return;
    }
    lastCheck = now;

    const DiscordIpcClient::Activity activity = makeActivity();
    if (!force && lastSentActivity && *lastSentActivity == activity && now - lastRefresh < presenceRefreshInterval) {
        return;
    }

    if (ipcClient->setActivity(activity)) {
        lastSentActivity = activity;
        lastRefresh = now;
    }
}

DiscordIpcClient::Activity DiscordPresence::makeActivity() const {
    DiscordIpcClient::Activity activity;
    activity.startTimestamp =
        sessionStart > 0 ? sessionStart : std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    SDK::ClientInstance* clientInstance = SDK::ClientInstance::get();
    if (!clientInstance || !clientInstance->minecraft || !clientInstance->minecraft->getLevel() ||
        !clientInstance->getLocalPlayer()) {
        activity.details = "Playing Minecraft Bedrock";
        activity.state = "In menus";
        return activity;
    }

    if (activeServer) {
        activity.details = "Playing Minecraft Bedrock";
        if (activeServer->tracksHiveGame && hiveGameModeCode) {
            std::string_view gameName = *hiveGameModeCode;
            for (const auto& [code, name] : hiveGameNames) {
                if (code == *hiveGameModeCode) {
                    gameName = name;
                    break;
                }
            }

            if (*hiveGameModeCode == "HUB") {
                activity.details = "In the Hub";
            } else if (hiveGameModeCode->starts_with("HUB-")) {
                const std::string_view hubCode = std::string_view(*hiveGameModeCode).substr(4);
                std::string_view hubName = hubCode;
                for (const auto& [code, name] : hiveGameNames) {
                    if (code == hubCode) {
                        hubName = name;
                        break;
                    }
                }
                activity.details = std::format("In the {} Hub", hubName);
            } else {
                activity.details = std::format("Playing {}", gameName);
            }
        }

        activity.state = std::string(activeServer->name);
        activity.largeImageKey = std::string(activeServer->logoKey);
        activity.largeImageText = std::string(activeServer->logoTooltip);
        activity.smallImageKey = "latite";
        activity.smallImageText = "Latite Client Logo";
        return activity;
    }

    SDK::RakNetConnector* connector = SDK::RakNetConnector::get();
    activity.details = "Playing Minecraft Bedrock";
    activity.state = "Singleplayer";
    if (connector) {
        if (!connector->featuredServer.empty()) {
            activity.state = connector->featuredServer;
        } else if (!connector->dns.empty()) {
            activity.state = connector->dns;
            if (connector->port != 19132) {
                activity.state += std::format(":{}", connector->port);
            }
        }
    }

    activity.largeImageKey = "minecraft";
    activity.largeImageText = "Minecraft Bedrock Logo";
    activity.smallImageKey = "latite";
    activity.smallImageText = "Latite Client Logo";
    return activity;
}
