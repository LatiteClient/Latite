#pragma once
#include "../../Module.h"
#include <mc/common/network/packet/TextPacket.h>

class Nickname : public Module {
public:
	Nickname();
	virtual ~Nickname() = default;

	void onClientTextPacket(Event& evG);
	void onGetFormattedNameTag(Event& evG);
private:
	ValueType nickname = TextValue(L"Nickname");
};