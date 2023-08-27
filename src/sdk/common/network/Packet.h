#pragma once
namespace SDK {
	class Packet {
		char pad[0x28];

	public:
		virtual ~Packet() {};
		virtual int getID() { return 0; };
		virtual std::string getName() { return ""; };
		virtual void write(void* stream) {};
		virtual void readExtended(void* stream) {};
		virtual bool allowBatch() { return false; };
		virtual void _read(void* stream) {};
	};
}