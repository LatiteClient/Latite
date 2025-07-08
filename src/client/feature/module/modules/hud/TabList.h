#pragma once
#include "../../HUDModule.h"

class TabList : public Module {
public:
	TabList();

	void onRenderOverlay(Event&);
	bool shouldHoldToToggle() override { return true; }
private:
	ValueType textCol = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType bgCol = ColorValue(0.f, 0.f, 0.f, 0.5f);
};

