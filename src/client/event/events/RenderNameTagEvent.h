#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class RenderNameTagEvent : public Event {
public:
	static const uint32_t hash = TOHASH(RenderNameTagEvent);

	RenderNameTagEvent(std::string* nametag) : nametag(nametag) {}

	[[nodiscard]] std::string* getNametag() { return nametag; }
private:
	std::string* nametag;
};