#include "pch.h"
#include <regex>
#include <algorithm>
#include "AutoQueue.h"
#include <sdk/common/network/packet/CommandRequestPacket.h>
#include "sdk/common/network/MinecraftPackets.h"
#include <sdk/common/network/packet/SetTitlePacket.h>

AutoQueue::AutoQueue() : Module("AutoQueue", LocalizeString::get("client.module.autoQueue.name"),
	LocalizeString::get("client.module.autoQueue.desc"), GAME, nokeybind) {
	addSetting("filterSensitiveInfo", LocalizeString::get("client.module.autoQueue.filterSensitiveInfo.name"),
		LocalizeString::get("client.module.autoQueue.filterSensitiveInfo.desc"), filterSensitiveInfo);
	addSetting("soloMode", LocalizeString::get("client.module.autoQueue.soloMode.name"),
		LocalizeString::get("client.module.autoQueue.soloMode.desc"), soloMode);
	listen<PacketReceiveEvent>(static_cast<EventListenerFunc>(&AutoQueue::onPacketRecieve));
	listen<ChatMessageEvent>(static_cast<EventListenerFunc>(&AutoQueue::onText));
}

std::wstring AutoQueue::extractHiveGamemode(std::wstring& message) {
	const std::wstring keyword = L"You are connected to server name";
	size_t keywordPos = message.find(keyword);
	std::wstring afterKeyword = message.substr(keywordPos + keyword.length());
	std::wstring result;

	for (char c : afterKeyword) {
		if (!std::isdigit(c)) {
			result += c;
		}
	}

	size_t start = result.find_first_not_of(L" \t");
	size_t end = result.find_last_not_of(L" \t");

	return result.substr(start, end - start + 1);
}

void AutoQueue::queueHiveGame() {
	SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();
	std::shared_ptr<SDK::Packet> pkt = SDK::MinecraftPackets::createPacket(SDK::PacketID::COMMAND_REQUEST);
	SDK::CommandRequestPacket* cmd = reinterpret_cast<SDK::CommandRequestPacket*>(pkt.get());
	cmd->applyCommand("/q " + util::WStrToStr(currentHiveGamemode));
	localPlayer->packetSender->sendToServer(pkt.get());
}

void AutoQueue::onPacketRecieve(Event& evG) {
	auto& ev = reinterpret_cast<PacketReceiveEvent&>(evG);
	SDK::LocalPlayer* localPlayer = SDK::ClientInstance::get()->getLocalPlayer();

	if (ev.getPacket()->getID() == SDK::PacketID::TEXT) {
		SDK::TextPacket* textPacket = reinterpret_cast<SDK::TextPacket*>(ev.getPacket());
		std::wstring chatMessage = util::StrToWStr(textPacket->str);

		if (textPacket->xboxUserId == "") {
			if (textPacket->str.find("You are connected to server name") != std::string::npos) {
				currentHiveGamemode = extractHiveGamemode(chatMessage);
			}

			if (std::get<BoolValue>(filterSensitiveInfo)) {
				if (textPacket->str.find("You are connected to") != std::string::npos) {
					supress = true;
				} else {
					supress = false;
				}
			}

			if (std::get<BoolValue>(soloMode)) {
				if (textPacket->str.find("You finished in") != std::string::npos) {
					queueHiveGame();
				}
			}

			if (textPacket->str.find("Game OVER!") != std::string::npos) {
				queueHiveGame();
			}
		}
	}

	if (ev.getPacket()->getID() == SDK::PacketID::CHANGE_DIMENSION) {
		if (!sentCmd) {
			std::shared_ptr<SDK::Packet> pkt = SDK::MinecraftPackets::createPacket(SDK::PacketID::COMMAND_REQUEST);
			SDK::CommandRequestPacket* cmd = reinterpret_cast<SDK::CommandRequestPacket*>(pkt.get());
			cmd->applyCommand("/connection");
			localPlayer->packetSender->sendToServer(pkt.get());
			sentCmd = true;
		} else {
			sentCmd = false;
		}

	}

	if (ev.getPacket()->getID() == SDK::PacketID::SET_TITLE) {
		SDK::SetTitlePacket* pkt = reinterpret_cast<SDK::SetTitlePacket*>(ev.getPacket());
		if (pkt->type == SDK::TitleType::Title) {
			std::string titleText = pkt->text;
			if (std::get<BoolValue>(soloMode)) {
				if (titleText.find("spectat") != std::string::npos) {
					queueHiveGame();
				}

				if (titleText.find("You died!") != std::string::npos) {
					queueHiveGame();
				}
			}
		}
	}
}

void AutoQueue::onText(Event& evG) {
	auto& ev = reinterpret_cast<ChatMessageEvent&>(evG);
	if (supress) {
		ev.setCancelled(true);
	}
}
