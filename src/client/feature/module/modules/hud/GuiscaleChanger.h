#pragma once
#include "client/feature/module/HUDModule.h"

class GuiscaleChanger : public Module {
public:
	GuiscaleChanger();
private:
	void onTick(Event& ev);
	ValueType guiscale = FloatValue(2.f);
};

