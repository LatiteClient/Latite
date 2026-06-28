#include "pch.h"
#include "PacketHooks.h"
#include "client/script/PluginManager.h"
#include <mc/common/network/MinecraftPackets.h>
#include <mc/common/network/packet/AddPlayerPacket.h>
#include <mc/common/network/packet/SetActorDataPacket.h>
#include <limits>
#include <type_traits>
#include <unordered_map>

namespace {
    std::shared_ptr<Hook> SetTitlePacketRead;
    std::shared_ptr<Hook> TextPacketRead;
    std::shared_ptr<Hook> SendToServerHook;
    std::shared_ptr<Hook> CreatePacketHook;

    constexpr size_t PacketHookArraySize =
        static_cast<size_t>(std::numeric_limits<std::underlying_type_t<SDK::PacketID>>::max()) + 1;

    std::array<std::shared_ptr<Hook>, PacketHookArraySize> PacketHookArray;
    std::unordered_map<uintptr_t, std::shared_ptr<Hook>> PacketHooksByVtableSlot;
}

void PacketHooks::PacketSender_sendToServer(SDK::PacketSender* sender, SDK::Packet* packet) {
    SendPacketEvent ev { packet };

    if (Eventing::get().dispatch(ev)) {
        return;
    }

    SendToServerHook->oFunc<decltype(&PacketSender_sendToServer)>()(sender, packet);
}

std::shared_ptr<SDK::Packet> PacketHooks::MinecraftPackets_createPacket(SDK::PacketID packetId) {
    auto genPacket = CreatePacketHook->oFunc<decltype(&MinecraftPackets_createPacket)>()(packetId);

    return genPacket;
}

void PacketHooks::PacketHandlerDispatcherInstance_handle(void* instance, void* networkIdentifier,
                                                         void* netEventCallback, std::shared_ptr<SDK::Packet>& packet) {
    if (!packet) return;

    auto packetId = packet->getID();
    auto hook = PacketHookArray[static_cast<size_t>(static_cast<std::underlying_type_t<SDK::PacketID>>(packetId))];
    if (!hook && instance) {
        auto** vft = *reinterpret_cast<void***>(instance);
        auto hookIt = PacketHooksByVtableSlot.find(reinterpret_cast<uintptr_t>(vft + 1));
        if (hookIt != PacketHooksByVtableSlot.end()) {
            hook = hookIt->second;
        }
    }
    if (!hook) return;

    const bool isMainThread = Latite::isMainThread();
    std::shared_ptr<SDK::Packet> postVanillaPacket = isMainThread ? packet : nullptr;

    if (isMainThread) {
        PacketReceiveEvent ev { packet.get() };
        Eventing::get().dispatch(ev);

        if (packetId == SDK::PacketID::MODAL_FORM_REQUEST) {
            auto pkt = std::static_pointer_cast<SDK::ModalFormRequestPacket>(packet);

            auto formId = PluginManager::Event::Value(L"formId");
            formId.val = (double)pkt->mFormId;

            auto formJson = PluginManager::Event::Value(L"formJson");
            formJson.val = util::StrToWStr(pkt->mFormJSON);

            PluginManager::Event sEv { L"modal-form-request", { formId, formJson }, false };
            if (Latite::getPluginManager().dispatchEvent(sEv)) return;
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
            v2.val = util::StrToWStr(pkt->text);

            PluginManager::Event ev(L"title", { v1, v2 }, true);
            if (Latite::getPluginManager().dispatchEvent(ev)) {
                pkt->type = SDK::TitleType::Clear;
            }
        } else if (packetId == SDK::PacketID::TEXT) {
            auto pkt = std::static_pointer_cast<SDK::TextPacket>(packet).get();

            PluginManager::Event::Value typ { L"type" };
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
            case SDK::TextPacketType::OBJECT_WHISPER:
                typ.val = L"object_whisper";
                break;
            case SDK::TextPacketType::TEXT_OBJECT:
                typ.val = L"text_object";
                break;
            case SDK::TextPacketType::TEXT_OBJECT_ANNOUNCEMENT:
                typ.val = L"text_object_announcement";
                break;
            }

            PluginManager::Event::Value val { L"message" };
            std::string* message = pkt->getMessage();
            val.val = util::StrToWStr(message ? *message : "");

            PluginManager::Event::Value val2 { L"sender" };
            std::string* sender = pkt->getAuthor();
            val2.val = util::StrToWStr(sender ? *sender : "");

            PluginManager::Event::Value val3 { L"xuid" };
            val3.val = util::StrToWStr(pkt->xboxUserId);

            PluginManager::Event::Value isChat { L"isChat" };
            isChat.val =
                (pkt->type == SDK::TextPacketType::CHAT || pkt->type == SDK::TextPacketType::RAW ||
                 pkt->type == SDK::TextPacketType::SYSTEM_MESSAGE || pkt->type == SDK::TextPacketType::WHISPER ||
                 pkt->type == SDK::TextPacketType::OBJECT_WHISPER || pkt->type == SDK::TextPacketType::ANNOUNCEMENT ||
                 pkt->type == SDK::TextPacketType::TEXT_OBJECT_ANNOUNCEMENT);

            PluginManager::Event sEv { L"receive-chat", { typ, val, val2, val3, isChat }, true };
            if (Latite::getPluginManager().dispatchEvent(sEv)) {
                return;
            }

            ClientTextEvent ev { pkt };
            if (Eventing::get().dispatch(ev)) {
                return;
            }
        } else if (packetId == SDK::PacketID::CHANGE_DIMENSION) {
            Latite::get().getNameTagCache().clearNetworkNameTags();
            PluginManager::Event sEv { L"change-dimension", {}, false };
            Latite::getPluginManager().dispatchEvent(sEv);
        } else if (packetId == SDK::PacketID::ADD_PLAYER) {
            SDK::AddPlayerPacket* addPlayer = static_cast<SDK::AddPlayerPacket*>(packet.get());
            uint64_t runtimeId = 0;
            std::string nameTag;
            if (addPlayer->tryGetNameTag(&runtimeId, &nameTag)) {
                Latite::get().getNameTagCache().recordNetworkNameTag(runtimeId, nameTag);
            }
        } else if (packetId == SDK::PacketID::SET_ENTITY_DATA) {
            SDK::SetActorDataPacket* setActorData = static_cast<SDK::SetActorDataPacket*>(packet.get());
            uint64_t runtimeId = 0;
            std::string nameTag;
            if (setActorData->tryGetNameTag(&runtimeId, &nameTag)) {
                Latite::get().getNameTagCache().recordNetworkNameTag(runtimeId, nameTag);
            }
        } else if (packetId == SDK::PacketID::SET_SCORE) {
            std::shared_ptr<SDK::SetScorePacket> pkt = std::static_pointer_cast<SDK::SetScorePacket>(packet);

            PluginManager::Event::Value data { L"data" };
            data.val = pkt->serialize();

            PluginManager::Event sEv { L"set-score", { data }, false };
            Latite::getPluginManager().dispatchEvent(sEv);
        } else if (packetId == SDK::PacketID::TRANSFER) {
            PluginManager::Event sEv { L"transfer", {}, false };
            Latite::getPluginManager().dispatchEvent(sEv);
        }
    }

    hook->oFunc<decltype(&PacketHandlerDispatcherInstance_handle)>()(instance, networkIdentifier, netEventCallback,
                                                                     packet);
}

PacketHooks::PacketHooks() {
    // CreatePacketHook = addHook(Signatures::MinecraftPackets_createPacket.result,
    //     MinecraftPackets_createPacket,
    //     "MinecraftPackets::createPacket");

    for (size_t i = 1; i < PacketHookArray.size(); i++) {
        auto pkt = SDK::MinecraftPackets::createPacket(
            static_cast<SDK::PacketID>(static_cast<std::underlying_type_t<SDK::PacketID>>(i)));
        if (pkt) {
            auto vft = *pkt->handler;
            auto const vtableSlot = reinterpret_cast<uintptr_t>(vft + 1);
            auto [hookIt, inserted] = PacketHooksByVtableSlot.try_emplace(vtableSlot);
            if (inserted) {
                hookIt->second = addTableSwapHook(vtableSlot, &PacketHandlerDispatcherInstance_handle, "Packet Hook");
            }
            PacketHookArray[i] = hookIt->second;
        }
    }
}

void PacketHooks::initPacketSender(SDK::PacketSender* sender) {
    uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(sender);
    SendToServerHook =
        addTableSwapHook((uintptr_t)(vtable + 2), PacketSender_sendToServer, "PacketSender::sendToServer");
}
