#pragma once
#include "../../Module.h"
#include "client/event/impl/GammaEvent.h"

class NoHurtCam : public Module {
public:
	NoHurtCam();

	void onBobHurt(Event& ev);
private:
};