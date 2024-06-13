#pragma once
#include "../../TextModule.h"

class Coordinates : public TextModule {
public:
	Coordinates();

	std::wstringstream text(bool isDefault, bool inEditor) override;
private:
	ValueType showBiome = BoolValue(true);
	ValueType showDimension = BoolValue(true);
};
