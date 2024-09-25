#pragma once
#include "Packet.h"

namespace SDK {
	class PacketSender {
		virtual ~PacketSender() = 0;
	public:
		virtual void send(Packet*) = 0;
		virtual void sendToServer(Packet* pkt) = 0;
	};
}
