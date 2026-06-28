#include "pch.h"
#include "Nickname.h"

Nickname::Nickname()
    : Module("Nickname", LocalizeString::get("client.module.nickname.name"),
             LocalizeString::get("client.module.nickname.desc"), GAME) {
    addSetting("nick", LocalizeString::get("client.module.nickname.name"),
               LocalizeString::get("client.module.nickname.newNick.desc"), this->nickname);

    listen<ClientTextEvent>((EventListenerFunc)&Nickname::onClientTextPacket);
    listen<RenderNameTagEvent>((EventListenerFunc)&Nickname::onRenderNameTag);
}

void Nickname::onClientTextPacket(Event& evG) {
    SDK::TextPacket* textPacket = reinterpret_cast<ClientTextEvent&>(evG).getTextPacket();

    if (!SDK::ClientInstance::get()->getLocalPlayer()) return;

    auto replaceAll = [](std::string& s, std::string from, std::string to) {
        if (!from.empty())
            for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
                s.replace(pos, from.size(), to);
        return s;
    };

    std::string newName = util::WStrToStr(std::get<TextValue>(this->nickname).str);
    std::string& currentPlayerName = SDK::ClientInstance::get()->getLocalPlayer()->playerName;
    if (std::string* author = textPacket->getAuthor()) {
        replaceAll(*author, currentPlayerName, newName);
    }
    if (std::string* message = textPacket->getMessage()) {
        replaceAll(*message, currentPlayerName, newName);
    }
}

void Nickname::onRenderNameTag(Event& evG) {
    RenderNameTagEvent& ev = reinterpret_cast<RenderNameTagEvent&>(evG);

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
