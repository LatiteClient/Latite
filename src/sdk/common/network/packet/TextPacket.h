#pragma once
#include "sdk/String.h"
#include "sdk/common/network/Packet.h"

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
		String source;
		String str;

	private:
		[[maybe_unused]] char pad[9 * 8];
	public:
		String xboxUserId, platformChatId;
		// theres probably more stuff
	private:
		[[maybe_unused]] char pad2[100];

	public:
		TextPacket();

		void chat(String const& message);
	};
}