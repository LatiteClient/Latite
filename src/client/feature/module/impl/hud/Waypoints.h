#pragma once

#include "../../Module.h"

class Waypoints : public Module {
public:
	Waypoints();
private:
	void onRenderLayer(Event& ev);
};