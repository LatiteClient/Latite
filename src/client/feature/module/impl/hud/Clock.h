#pragma once
#include "../../TextModule.h"

class Clock : public TextModule {
public:
	Clock();

	std::wstringstream text(bool isDefault, bool inEditor) override;
private:
	ValueType militaryTime = BoolValue(false);
};
