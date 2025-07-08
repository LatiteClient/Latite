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
		TEXT_OBJECT,
		OBJECT_WHISPER
	};

	class TextPacket : public Packet {
	public:
		TextPacketType type;
		std::string source;
		std::string str;
		std::optional<std::string> filteredMessage;
		std::vector<std::string> params;
		bool translationNeeded = false;
		std::string xboxUserId;
		std::string platformChatId;

		TextPacket() = default;
		
		void chat(std::string const& message);
	};
}