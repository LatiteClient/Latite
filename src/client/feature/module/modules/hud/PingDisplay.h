#pragma once
#include "../../TextModule.h"

class PingDisplay : public TextModule {
public:
	PingDisplay();

	std::wstringstream text(bool isDefault, bool inEditor) override;

	void onAvgPing(Event& ev);
private:
	int ping = 0;
};