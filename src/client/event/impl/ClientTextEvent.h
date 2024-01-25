#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include <sdk/common/network/packet/TextPacket.h>

class ClientTextEvent : public Event {
public:
	static const uint32_t hash = TOHASH(ClientTextEvent);

protected:
	SDK::TextPacket* textPacket;
public:

	SDK::TextPacket* getTextPacket() {
		return textPacket;
	}

	ClientTextEvent(SDK::TextPacket* textPacket)
		: textPacket(textPacket)
	{
	}
};