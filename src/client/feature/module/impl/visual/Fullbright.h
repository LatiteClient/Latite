#pragma once
#include "../../Module.h"
#include "client/event/impl/GammaEvent.h"

class Fullbright : public Module {
public:
	Fullbright();

	void onGamma(Event& ev);
private:
	Setting::Value gamma = FloatValue(25.f);
};