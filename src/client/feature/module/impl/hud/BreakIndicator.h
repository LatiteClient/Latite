#pragma once
#include "../../TextModule.h"

class BreakIndicator : public TextModule {
private:
	ValueType last = BoolValue(false);
public:
	BreakIndicator();
	virtual ~BreakIndicator();

private:
	std::wstringstream text(bool isDefault, bool inEditor) override;
};