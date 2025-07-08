#pragma once
#include "../../TextModule.h"

class SpeedDisplay : public TextModule {
public:
	SpeedDisplay();
private:
	void onTick(Event& ev);

	ValueType decimals = FloatValue(3.f);
	ValueType includeY = BoolValue(false);

	float speed;
protected:
	std::wstringstream text(bool isDefault, bool inEditor) override;
};