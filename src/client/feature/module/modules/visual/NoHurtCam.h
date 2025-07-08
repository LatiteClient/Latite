#pragma once
#include "../../Module.h"

class NoHurtCam : public Module {
public:
	NoHurtCam();

	void onBobHurt(Event& ev);
};