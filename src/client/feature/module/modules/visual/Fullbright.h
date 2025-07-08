#pragma once
#include "../../Module.h"
#include "client/event/events/GammaEvent.h"

class Fullbright : public Module {
public:
	Fullbright();

	void onGamma(Event& ev);
private:
	ValueType gamma = FloatValue(25.f);
};