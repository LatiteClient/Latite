#pragma once
#include "../Packet.h"
#include "sdk/String.h"
#include "sdk/Util.h"

namespace SDK {
	class CommandRequestPacket : public Packet {
		char pad[0x9C - sizeof(Packet)];
	public:
		CLASS_FIELD(int, unkInt1, 0x8); // should be 2
		CLASS_FIELD(int, unkInt2, 0xC); // should be 1
		CLASS_FIELD(int, type, 0x24);// should be 0
		CLASS_FIELD(String, command, 0x30); // includes '/'
		CLASS_FIELD(bool, unkBool, 0x94); //should be false

		CommandRequestPacket(std::string command);
	};
}
