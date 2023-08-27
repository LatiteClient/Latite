#pragma once
namespace SDK {
	class PacketSender {
		virtual ~PacketSender() = 0;
	public:
		virtual void send(class Packet&) = 0;
		virtual void sendToServer(class Packet& pkt) = 0;
	};
}