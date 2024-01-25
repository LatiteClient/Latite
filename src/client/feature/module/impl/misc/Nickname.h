#pragma once
#include "../../Module.h"
#include <sdk/common/network/packet/TextPacket.h>

class Nickname : public Module {
public:
	Nickname();
	virtual ~Nickname() = default;

	void onClientTextPacket(SDK::TextPacket* textPacket);
private:
	ValueType nickname = TextValue(L"Nickname");
};