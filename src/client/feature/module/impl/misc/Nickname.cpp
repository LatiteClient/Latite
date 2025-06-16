#include "pch.h"
#include "Nickname.h"

Nickname::Nickname() : Module("Nickname", LocalizeString::get("client.module.nickname.name"),
                              LocalizeString::get("client.module.nickname.desc"), GAME) {
	addSetting("nick", LocalizeString::get("client.module.nickname.name"),
               LocalizeString::get("client.module.nickname.newNick.desc"), this->nickname);

    listen<ClientTextEvent>((EventListenerFunc)&Nickname::onClientTextPacket);
    listen<GetFormattedNameTagEvent>((EventListenerFunc)&Nickname::onGetFormattedNameTag);
}

void Nickname::onClientTextPacket(Event& evG) {
    auto textPacket = reinterpret_cast<ClientTextEvent&>(evG).getTextPacket();

    if (!SDK::ClientInstance::get()->getLocalPlayer()) return;

    auto message = textPacket->str;
    auto source = textPacket->source;

    auto replaceAll = [](std::string& s, std::string from, std::string to) {
        if (!from.empty())
            for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
                s.replace(pos, from.size(), to);
        return s;
    };

    std::string newName = util::WStrToStr(std::get<TextValue>(this->nickname).str);
    auto& currentPlayerName = SDK::ClientInstance::get()->getLocalPlayer()->playerName;
    replaceAll(message, currentPlayerName, newName);
    replaceAll(source, currentPlayerName, newName);

    textPacket->str = message;
    textPacket->source = source;
}

void Nickname::onGetFormattedNameTag(Event& evG) {
    GetFormattedNameTagEvent& ev = reinterpret_cast<GetFormattedNameTagEvent&>(evG);
    *ev.getNametag() = util::WStrToStr(std::get<TextValue>(this->nickname).str);
}
