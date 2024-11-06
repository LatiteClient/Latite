#include "pch.h"
#include "PacketHooks.h"
#include "client/script/PluginManager.h"
#include <sdk/common/network/MinecraftPackets.h>

namespace {
	std::shared_ptr<Hook> SetTitlePacketRead;
	std::shared_ptr<Hook> TextPacketRead;
	std::shared_ptr<Hook> SendToServerHook;
	std::shared_ptr<Hook> CreatePacketHook;

	std::array<std::shared_ptr<Hook>, (size_t)SDK::PacketID::COUNT> PacketHookArray;
}

void PacketHooks::PacketSender_sendToServer(SDK::PacketSender* sender, SDK::Packet* packet) {
	SendPacketEvent ev{ packet };

	if (Eventing::get().dispatch(ev)) {
		return;
	}

	SendToServerHook->oFunc<decltype(&PacketSender_sendToServer)>()(sender, packet);
}

std::shared_ptr<SDK::Packet> PacketHooks::MinecraftPackets_createPacket(SDK::PacketID packetId) {
	auto genPacket = CreatePacketHook->oFunc<decltype(&MinecraftPackets_createPacket)>()(packetId);
	
	return genPacket;
}

void PacketHooks::PacketHandlerDispatcherInstance_handle(void* instance, void* networkIdentifier, void* netEventCallback, std::shared_ptr<SDK::Packet>& packet) {
	auto& hook = PacketHookArray[(size_t)packet->getID()];

	if (Latite::isMainThread()) {
		PacketReceiveEvent ev{ packet.get() };
		Eventing::get().dispatch(ev);

		auto packetId = packet->getID();

		if (packetId == SDK::PacketID::CHANGE_DIMENSION) {
			PluginManager::Event sEv{ XW("change-dimension"), {}, false };
			Latite::getPluginManager().dispatchEvent(sEv);
		}
		else if (packetId == SDK::PacketID::SET_SCORE) {
			auto pkt = std::static_pointer_cast<SDK::SetScorePacket>(packet);

			auto data = PluginManager::Event::Value(L"data");
			std::string json = SDK::makeJson(*pkt);
			data.val = util::StrToWStr(json);

			PluginManager::Event sEv{ XW("set-score"), { data }, false };
			Latite::getPluginManager().dispatchEvent(sEv);
		}
		else if (packetId == SDK::PacketID::TRANSFER) {
			PluginManager::Event sEv{ XW("transfer"), {}, false };
			Latite::getPluginManager().dispatchEvent(sEv);
		} else if (packetId == SDK::PacketID::SET_TITLE) {
			auto pkt = std::static_pointer_cast<SDK::SetTitlePacket>(packet);
			auto v1 = PluginManager::Event::Value(L"type");

			switch (pkt->type) {
			case SDK::TitleType::Clear:
				v1.val = L"clear";
				break;
			case SDK::TitleType::Reset:
				v1.val = L"reset";
				break;
			case SDK::TitleType::Title:
				v1.val = L"title";
				break;
			case SDK::TitleType::Subtitle:
				v1.val = L"subtitle";
				break;
			case SDK::TitleType::Actionbar:
				v1.val = L"actionbar";
				break;
			case SDK::TitleType::Times:
				v1.val = L"times";
				break;
			case SDK::TitleType::TitleRaw:
				v1.val = L"titleraw";
				break;
			case SDK::TitleType::SubtitleRaw:
				v1.val = L"subtitleraw";
				break;
			case SDK::TitleType::ActionbarRaw:
				v1.val = L"actionbarraw";
				break;
			default:
				v1.val = L"unknown";
				break;
			}
			auto v2 = PluginManager::Event::Value(L"text");
			v2.val = util::StrToWStr(pkt->text.getCStr());

			PluginManager::Event ev(L"title", { v1, v2 }, true);
			if (Latite::getPluginManager().dispatchEvent(ev)) {
				pkt->type = SDK::TitleType::Clear;
			}
		}
		else if (packetId == SDK::PacketID::TEXT) {
			auto pkt = std::static_pointer_cast<SDK::TextPacket>(packet).get();

			PluginManager::Event::Value typ{ L"type" };
			typ.val = L"Unknown";
			switch (pkt->type) {
			case SDK::TextPacketType::RAW:
				typ.val = L"raw";
				break;
			case SDK::TextPacketType::CHAT:
				typ.val = L"chat";
				break;
			case SDK::TextPacketType::TRANSLATION:
				typ.val = L"translation";
				break;
			case SDK::TextPacketType::JUKEBOX_POPUP:
				typ.val = L"jukebox";
				break;
			case SDK::TextPacketType::TIP:
				typ.val = L"tip";
				break;
			case SDK::TextPacketType::SYSTEM_MESSAGE:
				typ.val = L"system_message";
				break;
			case SDK::TextPacketType::WHISPER:
				typ.val = L"whisper";
				break;
			case SDK::TextPacketType::ANNOUNCEMENT:
				typ.val = L"announcement";
				break;
			case SDK::TextPacketType::TEXT_OBJECT:
				typ.val = L"text_object";
				break;
			case SDK::TextPacketType::OBJECT_WHISPER:
				typ.val = L"object_whisper";
				break;
			}

			PluginManager::Event::Value val{ L"message" };
			val.val = util::StrToWStr(pkt->str);

			PluginManager::Event::Value val2{ L"sender" };
			val2.val = util::StrToWStr(pkt->source);

			PluginManager::Event::Value val3{ L"xuid" };
			val3.val = util::StrToWStr(pkt->xboxUserId);

			PluginManager::Event::Value isChat{ L"isChat" };
			isChat.val = (pkt->type == SDK::TextPacketType::CHAT || pkt->type == SDK::TextPacketType::RAW
				|| pkt->type == SDK::TextPacketType::SYSTEM_MESSAGE || pkt->type == SDK::TextPacketType::WHISPER
				|| pkt->type == SDK::TextPacketType::OBJECT_WHISPER || pkt->type == SDK::TextPacketType::ANNOUNCEMENT);

			PluginManager::Event sEv{ L"receive-chat", { typ, val, val2, val3, isChat }, true };
			if (Latite::getPluginManager().dispatchEvent(sEv)) {
				return;
			}

			ClientTextEvent ev{ pkt };
			Eventing::get().dispatch(ev);
		}
	}
	hook->oFunc<decltype(&PacketHandlerDispatcherInstance_handle)>()(instance, networkIdentifier, netEventCallback, packet);
}

PacketHooks::PacketHooks() {
	//CreatePacketHook = addHook(Signatures::MinecraftPackets_createPacket.result,
	//    MinecraftPackets_createPacket,
	//    "MinecraftPackets::createPacket");

	for (uint8_t i = 1; i < (uint8_t)SDK::PacketID::COUNT; i++) {
		auto pkt = SDK::MinecraftPackets::createPacket((SDK::PacketID)i);
		if (pkt) {
			auto vft = *pkt->handler;
			PacketHookArray[i] = addTableSwapHook((uintptr_t)(vft + 1), &PacketHandlerDispatcherInstance_handle, "Packet Hook");
		}
	}
}

void PacketHooks::initPacketSender(SDK::PacketSender* sender) {
	uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(sender);
	SendToServerHook = addTableSwapHook((uintptr_t)(vtable + 2), PacketSender_sendToServer, "PacketSender::sendToServer");
}
