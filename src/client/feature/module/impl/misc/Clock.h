#pragma once
#include "../../TextModule.h"

class Clock : public TextModule {
public:
	Clock();

	std::wstringstream text(bool isDefault, bool inEditor) override;
	std::string getTimeString();
private:
	ValueType militaryTime = BoolValue(false);
	ValueType showDate = BoolValue(false);
};
