#pragma once
#include "../../TextModule.h"

class FrameTimeDisplay : public TextModule {
public:
	FrameTimeDisplay();

	std::wstringstream text(bool isDefault, bool inEditor) override;
private:
	ValueType interval = FloatValue(100.f);

	std::chrono::steady_clock::time_point lastUpdate;
	float displayedValue = -1.0f;
};