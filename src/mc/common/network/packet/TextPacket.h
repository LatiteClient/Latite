#pragma once
#include "mc/common/network/Packet.h"

namespace SDK {
    enum class TextPacketType : unsigned char {
        RAW,
        CHAT,
        TRANSLATION,
        POPUP,
        JUKEBOX_POPUP,
        TIP,
        SYSTEM_MESSAGE,
        WHISPER,
        ANNOUNCEMENT,
        OBJECT_WHISPER,
        TEXT_OBJECT,
        TEXT_OBJECT_ANNOUNCEMENT
    };

    class TextPacket : public Packet {
    public:
        bool translationNeeded = false;
        std::string xboxUserId;
        std::string platformChatId;
        std::optional<std::string> filteredMessage;
        TextPacketType type;
        std::string str;
        std::variant<std::monostate, std::string, std::vector<std::string>> data;

        TextPacket() = default;

        uint8_t getBodyVariant() const;
        std::string* getMessage();
        const std::string* getMessage() const;
        std::string* getAuthor();
        const std::string* getAuthor() const;
        void chat(std::string const& message);
    };
}
