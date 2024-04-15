#pragma once
namespace SDK {
	enum class PacketID : int32_t {
		NONE = 0,
		TEXT = 0x9,
		SET_TITLE = 0x58,
		COMMAND_REQUEST = 0x4D,
	};

	class Packet {
		char pad[0x28];

	public:
		virtual ~Packet() {};
		virtual PacketID getID() { return PacketID::NONE; };
		virtual std::string getName() { return ""; };
		virtual void write(void* stream) {};
		virtual void readExtended(void* stream) {};
		virtual bool allowBatch() { return false; };
		virtual void _read(void* stream) {};
	};
}