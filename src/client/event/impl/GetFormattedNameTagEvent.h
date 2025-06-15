#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"

class GetFormattedNameTagEvent : public Event {
public:
	static const uint32_t hash = TOHASH(GetFormattedNameTagEvent);

	GetFormattedNameTagEvent(std::string* nametag, SDK::LocalPlayer* player) : nametag(nametag), player(player) {}

	[[nodiscard]] SDK::LocalPlayer* getPlayer() { return player; }
	[[nodiscard]] std::string* getNametag() { return nametag; }
private:
	std::string* nametag;
	SDK::LocalPlayer* player;
};