#include "pch.h"
#include "Nickname.h"

Nickname::Nickname() : Module("Nickname", "Nickname", "Sets your nickname in the chat.", GAME) {
	addSetting("nick", "Nickname", "Your new nickname.", this->nickname);

    listen<ClientTextEvent>((EventListenerFunc)Nickname::onClientTextPacket);
}

void Nickname::onClientTextPacket(SDK::TextPacket* textPacket) {
    if (!SDK::ClientInstance::get()->getLocalPlayer()) return;

    auto message = textPacket->str.str();
    auto source = textPacket->source.str();

    auto replaceAll = [](std::string& s, std::string x, std::string y) {
        size_t pos = 0;
        while (pos += y.length()) {
            pos = s.find(x, pos);
            if (pos == std::string::npos) {
                break;
            }

            s.replace(pos, x.length(), y);
        }
    };

    std::string newName = util::WStrToStr(std::get<TextValue>(this->nickname).str);
    auto& currentPlayerName = SDK::ClientInstance::get()->getLocalPlayer()->playerName;
    replaceAll(message, currentPlayerName, newName);
    replaceAll(source, currentPlayerName, newName);

    textPacket->str.setString(message.c_str());
    textPacket->source.setString(source.c_str());
}
