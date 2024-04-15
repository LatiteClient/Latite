#pragma once
namespace SDK {
	enum class PacketID : uint8_t {
		NONE = 0,
		TEXT = 0x9,
		TRANSFER = 0x55,
		SET_TITLE = 0x58,
		COMMAND_REQUEST = 0x4D,
		TOAST_REQUEST = 0xBA,
		COUNT,
	};

	class Packet {
	public:
		int32_t priority = 2;
		int32_t reliability = 1;
		uint8_t subClientId = 0;
		bool isHandled = false;
		void* unknown = nullptr;
		void*** handler = nullptr;
		int32_t compressibility = 0;

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