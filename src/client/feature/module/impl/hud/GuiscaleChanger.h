#pragma once
#include "client/feature/module/HUDModule.h"
#include "client/event/impl/TickEvent.h"

class GuiscaleChanger : public Module {
public:
	GuiscaleChanger();
private:
	void onTick(Event& ev);
	ValueType guiscale = FloatValue(2.f);
};

