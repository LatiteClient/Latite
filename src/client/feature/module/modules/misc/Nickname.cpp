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

    auto str = textPacket->str;

    auto replaceAll = [](std::string& s, std::string from, std::string to) {
        if (!from.empty())
            for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
                s.replace(pos, from.size(), to);
        return s;
    };

    std::string newName = util::WStrToStr(std::get<TextValue>(this->nickname).str);
    auto& currentPlayerName = SDK::ClientInstance::get()->getLocalPlayer()->playerName;
    replaceAll(str, currentPlayerName, newName);

    textPacket->str = str;

    if (std::holds_alternative<std::string>(textPacket->data)) {
        auto data = std::get<std::string>(textPacket->data);
        replaceAll(data, currentPlayerName, newName);
        textPacket->data = data;
    }
}

void Nickname::onGetFormattedNameTag(Event& evG) {
    GetFormattedNameTagEvent& ev = reinterpret_cast<GetFormattedNameTagEvent&>(evG);

    std::string nickname = util::WStrToStr(std::get<TextValue>(this->nickname).str);

    if (nickname.empty() || !SDK::ClientInstance::get()->getLocalPlayer()) {
        return;
    }

    std::string& currentNametag = *ev.getNametag();
    const std::string& originalName = SDK::ClientInstance::get()->getLocalPlayer()->playerName;

    auto replaceAll = [](std::string& s, std::string from, std::string to) {
        if (!from.empty())
            for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
                s.replace(pos, from.size(), to);
        return s;
    };

    replaceAll(currentNametag, originalName, nickname);
}
