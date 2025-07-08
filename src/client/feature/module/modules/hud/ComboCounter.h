#pragma once
#include <client/feature/module/TextModule.h>

class ComboCounter : public TextModule {
public:
	ComboCounter();
	virtual ~ComboCounter() {}

	std::wstringstream text(bool isDefault, bool inEditor) override;

	void onAttack(Event& ev);
	void onPacketReceive(Event& ev);
	void onTick(Event& ev);
private:
	int combo = 0;
	std::chrono::system_clock::time_point lastHurt{};
	uint64_t lastRuntimeId = 0;
	bool hasHit = false;
};