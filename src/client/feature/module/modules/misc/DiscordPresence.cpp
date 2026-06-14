#include "pch.h"
#include "DiscordPresence.h"

#include "client/Latite.h"
#include "client/event/events/ChatMessageEvent.h"
#include "client/event/events/SendPacketEvent.h"
#include "client/event/events/UpdateEvent.h"
#include "mc/common/network/MinecraftPackets.h"
#include "mc/common/network/packet/CommandRequestPacket.h"
#include "mc/common/network/RakNetConnector.h"

#include <cctype>
#include <utility>

namespace {
	struct PresenceDetails {
		std::string_view address;
		std::string_view name;
		std::string_view logoKey;
		std::string_view logoTooltip;
	};

	constexpr std::array knownServers = {
		PresenceDetails{ "hivebedrock.network", "The Hive", "thehive", "The Hive Logo" },
		PresenceDetails{ "cubecraft.net", "CubeCraft", "cubecraft", "CubeCraft Games Logo" },
		PresenceDetails{ "play.galaxite.net", "Galaxite", "galaxite", "Galaxite Network Logo" },
		PresenceDetails{ "zeqa.net", "Zeqa", "zeqa", "Zeqa Practice Logo" },
		PresenceDetails{ "nethergames.org", "NetherGames", "nethergames", "NetherGames Network Logo" }
	};

	constexpr std::array hiveGameNames = {
		std::pair<std::string_view, std::string_view>{ "WARS", "Treasure Wars" },
		std::pair<std::string_view, std::string_view>{ "DR", "DeathRun" },
		std::pair<std::string_view, std::string_view>{ "HIDE", "Hide and Seek" },
		std::pair<std::string_view, std::string_view>{ "SG", "Survival Games" },
		std::pair<std::string_view, std::string_view>{ "MURDER", "Murder Mystery" },
		std::pair<std::string_view, std::string_view>{ "SKY", "SkyWars" },
		std::pair<std::string_view, std::string_view>{ "CTF", "Capture the Flag" },
		std::pair<std::string_view, std::string_view>{ "DROP", "Block Drop" },
		std::pair<std::string_view, std::string_view>{ "GROUND", "Ground Wars" },
		std::pair<std::string_view, std::string_view>{ "BUILD", "Build Battle" },
		std::pair<std::string_view, std::string_view>{ "PARTY", "Block Party" },
		std::pair<std::string_view, std::string_view>{ "BRIDGE", "The Bridge" },
		std::pair<std::string_view, std::string_view>{ "GRAV", "Gravity" },
		std::pair<std::string_view, std::string_view>{ "BED", "BedWars" }
	};

	const PresenceDetails* findServerPresence(std::string_view serverAddress) {
		for (const PresenceDetails& server : knownServers) {
			if (serverAddress.find(server.address) != std::string_view::npos) {
				return &server;
			}
		}

		return nullptr;
	}

	std::optional<std::string_view> findHiveGameName(std::string_view gameCode) {
		for (const auto& game : hiveGameNames) {
			if (game.first == gameCode) {
				return game.second;
			}
		}

		return std::nullopt;
	}

	void useMinecraftAssets(DiscordIpcClient::Activity& activity) {
		activity.largeImageKey = "minecraft";
		activity.largeImageText = "Minecraft Bedrock Logo";
		activity.smallImageKey = "latite";
		activity.smallImageText = "Latite Client Logo";
	}

	void useLatiteAsset(DiscordIpcClient::Activity& activity) {
		activity.largeImageKey = "latite";
		activity.largeImageText = "Latite Client Logo";
		activity.smallImageKey.clear();
		activity.smallImageText.clear();
	}

	std::int64_t nowUnixSeconds() {
		return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}

	std::string stripMinecraftFormatting(std::string_view value) {
		std::string out;
		out.reserve(value.size());

		for (size_t i = 0; i < value.size(); i++) {
			const auto ch = static_cast<unsigned char>(value[i]);
			if (ch == 0xC2 && i + 2 < value.size() && static_cast<unsigned char>(value[i + 1]) == 0xA7) {
				i += 2;
				continue;
			}

			if (ch == 0xA7 && i + 1 < value.size()) {
				i++;
				continue;
			}

			out.push_back(value[i]);
		}

		return out;
	}

	std::string trimWhitespace(std::string_view value) {
		size_t start = 0;
		while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
			start++;
		}

		size_t end = value.size();
		while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
			end--;
		}

		return std::string(value.substr(start, end - start));
	}

	std::optional<std::string> parseHiveGameModeCode(std::string_view message) {
		constexpr std::string_view prefix = "You are connected to server name ";
		if (!message.starts_with(prefix)) {
			return std::nullopt;
		}

		std::string serverName = trimWhitespace(message.substr(prefix.size()));
		while (!serverName.empty() && std::isdigit(static_cast<unsigned char>(serverName.back()))) {
			serverName.pop_back();
		}

		serverName = trimWhitespace(serverName);
		if (serverName.empty()) {
			return std::nullopt;
		}

		return serverName;
	}

	bool isHiveConnectionMessage(std::string_view message) {
		return message.starts_with("You are connected to internal IP ")
			|| message.starts_with("You are connected to public IP ")
			|| message.starts_with("You are connected to server name ")
			|| message.starts_with("You are connected to server ");
	}

	bool isHiveConnectionCommand(std::string_view command) {
		return util::ToLower(trimWhitespace(command)) == "/connection";
	}

	std::string getHivePresenceDetails(std::string_view gameCode) {
		if (gameCode == "HUB") {
			return "In the Hub";
		}

		constexpr std::string_view hubPrefix = "HUB-";
		if (gameCode.starts_with(hubPrefix)) {
			const std::string_view hubGameCode = gameCode.substr(hubPrefix.size());
			if (std::optional<std::string_view> gameName = findHiveGameName(hubGameCode)) {
				return std::format("In the {} Hub", *gameName);
			}

			return std::format("In the {} Hub", hubGameCode);
		}

		if (std::optional<std::string_view> gameName = findHiveGameName(gameCode)) {
			return std::format("Playing {}", *gameName);
		}

		return std::format("Playing {}", gameCode);
	}
}

DiscordPresence::DiscordPresence() : Module(
	"DiscordPresence",
	LocalizeString::get("client.module.discordPresence.name"),
	LocalizeString::get("client.module.discordPresence.desc"),
	GAME
) {
	listen<UpdateEvent>(static_cast<EventListenerFunc>(&DiscordPresence::onUpdate));
	listen<ChatMessageEvent>(static_cast<EventListenerFunc>(&DiscordPresence::onChatMessage), false, 10);
	listen<SendPacketEvent>(static_cast<EventListenerFunc>(&DiscordPresence::onSendPacket), false, 10);
}

DiscordPresence::~DiscordPresence() {
	DiscordPresence::onDisable();
}

void DiscordPresence::onEnable() {
	if (sessionStart <= 0) {
		sessionStart = nowUnixSeconds();
	}
	lastCheck = {};
	lastRefresh = {};
	lastHiveConnectionProbe = {};
	lastUserHiveConnectionCommand = {};
	pendingHiveConnectionMessages = 0;
	sendingHiveConnectionProbe = false;
	lastSentActivity.reset();
	hiveGameModeCode.reset();
	ipcClient.emplace(discordApplicationId);
	DiscordPresence::publishPresence(true);
}

void DiscordPresence::onDisable() {
	if (ipcClient) {
		ipcClient->clearActivity();
		ipcClient.reset();
	}

	lastSentActivity.reset();
	hiveGameModeCode.reset();
	lastHiveConnectionProbe = {};
	lastUserHiveConnectionCommand = {};
	pendingHiveConnectionMessages = 0;
	sendingHiveConnectionProbe = false;
}

void DiscordPresence::onUpdate(Event&) {
	DiscordPresence::publishPresence(false);
}

void DiscordPresence::onChatMessage(Event& evG) {
	const bool hiddenProbeResponse = hasPendingHiveConnectionProbe();
	const bool userConnectionResponse = lastUserHiveConnectionCommand != std::chrono::steady_clock::time_point{}
		&& std::chrono::steady_clock::now() - lastUserHiveConnectionCommand <= hiveConnectionProbeResponseWindow;

	ChatMessageEvent& ev = reinterpret_cast<ChatMessageEvent&>(evG);
	const std::string message = trimWhitespace(stripMinecraftFormatting(ev.getMessage()));
	if (!isHiveConnectionMessage(message)) {
		return;
	}

	if (std::optional<std::string> modeCode = parseHiveGameModeCode(message)) {
		if (hiveGameModeCode != modeCode) {
			hiveGameModeCode = std::move(modeCode);
			lastSentActivity.reset();
			lastCheck = {};
		}
	}

	if (hiddenProbeResponse) {
		if (pendingHiveConnectionMessages > 0) {
			pendingHiveConnectionMessages--;
		}
		ev.setCancelled(true);
	} else if (!userConnectionResponse) {
		lastUserHiveConnectionCommand = {};
	}
}

void DiscordPresence::onSendPacket(Event& evG) {
	SendPacketEvent& ev = reinterpret_cast<SendPacketEvent&>(evG);
	if (ev.getPacket()->getID() != SDK::PacketID::COMMAND_REQUEST) {
		return;
	}

	const SDK::CommandRequestPacket* cmd = reinterpret_cast<SDK::CommandRequestPacket*>(ev.getPacket());
	if (!isHiveConnectionCommand(cmd->command)) {
		return;
	}

	if (sendingHiveConnectionProbe) {
		return;
	}

	pendingHiveConnectionMessages = 0;
	lastUserHiveConnectionCommand = std::chrono::steady_clock::now();
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

	const DiscordIpcClient::Activity activity = DiscordPresence::makeActivity();
	bool shouldRefresh = false;

	if (!lastSentActivity) {
		shouldRefresh = true;
	} else if (*lastSentActivity != activity) {
		shouldRefresh = true;
	} else if (now - lastRefresh >= presenceRefreshInterval) {
		shouldRefresh = true;
	}

	if (!force && !shouldRefresh) {
		return;
	}

	if (ipcClient->setActivity(activity)) {
		lastSentActivity = activity;
		lastRefresh = now;
	}
}

DiscordIpcClient::Activity DiscordPresence::makeActivity() {
	DiscordIpcClient::Activity activity;
	activity.startTimestamp = sessionStart > 0 ? sessionStart : nowUnixSeconds();

	SDK::ClientInstance* clientInstance = SDK::ClientInstance::get();
	if (!clientInstance || !clientInstance->minecraft || !clientInstance->minecraft->getLevel() || !clientInstance->getLocalPlayer()) {
		hiveGameModeCode.reset();
		lastHiveConnectionProbe = {};
		pendingHiveConnectionMessages = 0;
		activity.details = "Playing Minecraft Bedrock";
		activity.state = "In menus";

		useLatiteAsset(activity);
		return activity;
	}

	SDK::RakNetConnector* connector = SDK::RakNetConnector::get();
	if (connector) {
		const std::string serverAddress = !connector->dns.empty() ? connector->dns : connector->ipAddress;
		if (const PresenceDetails* server = findServerPresence(serverAddress)) {
			activity.details = hiveGameModeCode && server->name == "The Hive"
				? getHivePresenceDetails(*hiveGameModeCode)
				: "Playing Minecraft Bedrock";
			activity.state = std::string(server->name);

			if (server->name == "The Hive") {
				requestHiveConnectionInfo(std::chrono::steady_clock::now());
			} else {
				hiveGameModeCode.reset();
				lastHiveConnectionProbe = {};
				pendingHiveConnectionMessages = 0;
			}

			activity.largeImageKey = std::string(server->logoKey);
			activity.largeImageText = std::string(server->logoTooltip);
			activity.smallImageKey = "latite";
			activity.smallImageText = "Latite Client Logo";
			return activity;
		}
	}

	hiveGameModeCode.reset();
	lastHiveConnectionProbe = {};
	pendingHiveConnectionMessages = 0;
	activity.details = "Playing Minecraft Bedrock";
	activity.state = getPresenceState();
	useMinecraftAssets(activity);
	return activity;
}

std::string DiscordPresence::getPresenceState() const {
    SDK::RakNetConnector* connector = SDK::RakNetConnector::get();
	// path for generic servers
    if (connector) {
        if (!connector->featuredServer.empty()) {
            return connector->featuredServer;
        }

        if (!connector->dns.empty()) {
            std::string server = connector->dns;
            if (connector->port != 19132) {
                server += std::format(":{}", connector->port);
            }
            return server;
        }
    }

    return "Singleplayer";
}

void DiscordPresence::requestHiveConnectionInfo(std::chrono::steady_clock::time_point now) {
	if (hasPendingHiveConnectionProbe()) {
		return;
	}

	if (lastUserHiveConnectionCommand != std::chrono::steady_clock::time_point{}
		&& now - lastUserHiveConnectionCommand < hiveUserConnectionBackoff) {
		return;
	}

	if (now - lastHiveConnectionProbe < hiveConnectionProbeInterval) {
		return;
	}

	SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();
	if (!localPlayer || !localPlayer->packetSender) {
		return;
	}

	std::shared_ptr<SDK::Packet> pkt = SDK::MinecraftPackets::createPacket(SDK::PacketID::COMMAND_REQUEST);
	if (!pkt) {
		return;
	}

	SDK::CommandRequestPacket* cmd = reinterpret_cast<SDK::CommandRequestPacket*>(pkt.get());
	cmd->applyCommand("/connection");

	lastHiveConnectionProbe = now;
	pendingHiveConnectionMessages = 3;
	sendingHiveConnectionProbe = true;
	localPlayer->packetSender->sendToServer(pkt.get());
	sendingHiveConnectionProbe = false;
}

bool DiscordPresence::hasPendingHiveConnectionProbe() const {
	if (lastHiveConnectionProbe == std::chrono::steady_clock::time_point{}) {
		return false;
	}

	return pendingHiveConnectionMessages > 0
		&& std::chrono::steady_clock::now() - lastHiveConnectionProbe <= hiveConnectionProbeResponseWindow;
}
