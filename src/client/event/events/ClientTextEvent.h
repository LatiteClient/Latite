#include "client/event/Event.h"
#include "util/Crypto.h"
#include <mc/common/network/packet/TextPacket.h>

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