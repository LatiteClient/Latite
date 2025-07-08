#pragma once
#include "../../TextModule.h"

class ToggleSprintSneak : public TextModule {
public:
	ToggleSprintSneak();

	void onTick(Event& ev);
	void beforeMove(Event& ev);
	void afterMove(Event& ev);
	void onKey(Event& ev);

private:
	ValueType label = BoolValue(true);
	ValueType sprint = BoolValue(true);
	ValueType alwaysSprint = BoolValue(false);
	ValueType sneak = BoolValue(false);

	static constexpr int sprint_mode_normal = 0;
	static constexpr int sprint_mode_raw = 1;
	EnumData sprintMode;

	bool toggleSprinting = false;
	bool toggleSneaking = false;
	bool lastSprintKey = false;
	bool lastSneakKey = false;
	bool realSneaking = false;
	bool realSprint = false;

	std::wstring left = L"";
	std::wstring right = L"";
protected:
	std::wstringstream text(bool isDefault, bool inEditor) override;
	bool isActive() override;
};