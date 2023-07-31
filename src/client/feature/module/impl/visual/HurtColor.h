#pragma once
#include "../../Module.h"
#include "client/event/impl/GammaEvent.h"

class HurtColor : public Module {
public:
	HurtColor();

	void onActorOverlay(Event& ev);
private:
	Setting::Value color = ColorValue(1.f, 0.f, 0.f, 0.6f);
};