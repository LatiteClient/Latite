#pragma once
#include "../../TextModule.h"

class FPSCounter : public TextModule {
public:
	FPSCounter();

	std::wstringstream text(bool isDefault, bool inEditor) override;
};