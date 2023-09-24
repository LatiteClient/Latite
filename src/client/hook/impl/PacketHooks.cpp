#include "pch.h"
#include "PacketHooks.h"
#include "client/script/ScriptManager.h"

namespace {
    std::shared_ptr<Hook> SetTitlePacketRead;
    std::shared_ptr<Hook> TextPacketRead;
}

void* PacketHooks::SetTitlePacket_readExtended(SDK::SetTitlePacket* pkt, void* b, void* c) {
    auto res = SetTitlePacketRead->oFunc<decltype(&SetTitlePacket_readExtended)>()(pkt, b, c);
    {
        auto v1 = ScriptManager::Event::Value(L"type");

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
        auto v2 = ScriptManager::Event::Value(L"text");
        v2.val = util::StrToWStr(pkt->text.getCStr());

        ScriptManager::Event ev(L"title", {v1, v2}, true);
        if (Latite::getScriptManager().dispatchEvent(ev)) {
            pkt->type = SDK::TitleType::Clear;
        }
    }
    return res;
}

void* PacketHooks::TextPacket_read(SDK::TextPacket* pkt, void* b, void* c) {
    auto res = TextPacketRead->oFunc<decltype(&TextPacket_read)>()(pkt, b, c);
    {
        JsContextRef ctx;
        JS::JsGetCurrentContext(&ctx);
        if (ctx != 0) { // This is the jankiest way possible to see if its the server or not

            ScriptManager::Event::Value typ{L"type"};
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

            ScriptManager::Event::Value val{L"message"};
            val.val = util::StrToWStr(pkt->str);

            ScriptManager::Event::Value val2{L"sender"};
            val2.val = util::StrToWStr(pkt->source);

            ScriptManager::Event::Value val3{L"xuid"};
            val3.val = util::StrToWStr(pkt->xboxUserId);

            ScriptManager::Event::Value isChat{L"isChat"};
            isChat.val = (pkt->type == SDK::TextPacketType::CHAT || pkt->type == SDK::TextPacketType::RAW
                || pkt->type == SDK::TextPacketType::SYSTEM_MESSAGE || pkt->type == SDK::TextPacketType::WHISPER
                || pkt->type == SDK::TextPacketType::OBJECT_WHISPER || pkt->type == SDK::TextPacketType::ANNOUNCEMENT);

            ScriptManager::Event ev{L"receive-chat", { typ, val, val2, val3, isChat }, false};
            if (Latite::getScriptManager().dispatchEvent(ev)) {
                pkt->type = SDK::TextPacketType::JUKEBOX_POPUP;
                pkt->str.setString("");
                pkt->source.setString("");
            }
        }
    }
    return res;
}

PacketHooks::PacketHooks() {
    if (Signatures::Vtable::SetTitlePacket.result) {
        auto vfunc = reinterpret_cast<uintptr_t*>(Signatures::Vtable::SetTitlePacket.result);
        SetTitlePacketRead = addTableSwapHook((uintptr_t)(vfunc + 4), SetTitlePacket_readExtended, "SetTitlePacket::readExtended");
    }

    if (Signatures::Vtable::TextPacket.result) {
        auto vfunc = reinterpret_cast<uintptr_t*>(Signatures::Vtable::TextPacket.result);
        TextPacketRead = addTableSwapHook((uintptr_t)(vfunc + 4), TextPacket_read, "TextPacket::read");
    }
}
