#pragma once
#include "../../TextModule.h"

class CustomCoordinates : public TextModule {
public:
	CustomCoordinates();

	std::wstringstream text(bool isDefault, bool inEditor) override;
private:
	ValueType showDimension = BoolValue(false);
};
