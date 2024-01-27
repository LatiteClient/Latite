#pragma once
#include "../../TextModule.h"

class ReachDisplay : public TextModule {
public:
	ReachDisplay();

	std::wstringstream text(bool isDefault, bool inEditor) override;

	void onAttack(Event& ev);
private:
	float reach = 0.f;
	ValueType decimals = FloatValue(3.f);

	std::chrono::system_clock::time_point lastAttack{};
};