#pragma once
#include "../Packet.h"
#include "sdk/String.h"

namespace SDK {

	class ModalFormRequestPacket : public Packet {
	public:
		uint8_t mFormId;
		std::string mFormJSON;
	};
}