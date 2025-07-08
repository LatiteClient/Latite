#pragma once
#include "../../Module.h"
#include "client/event/events/GammaEvent.h"

class HurtColor : public Module {
public:
	HurtColor();

	void onActorOverlay(Event& ev);
private:
	ValueType color = ColorValue(1.f, 0.f, 0.f, 0.6f);
};