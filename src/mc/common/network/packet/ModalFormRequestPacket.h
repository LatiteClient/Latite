#pragma once
#include "../Packet.h"

namespace SDK {

	class ModalFormRequestPacket : public Packet {
	public:
		uint8_t mFormId;
		std::string mFormJSON;
	};
}