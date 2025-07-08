#pragma once
#include "../../Module.h"

class CommandShortcuts : public Module {
public:
	CommandShortcuts();
	virtual ~CommandShortcuts() {};

	void onPacket(Event& evG);
};