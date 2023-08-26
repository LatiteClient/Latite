#pragma once
#include "../../Module.h"

class DynamicLight : public Module {
public:
	DynamicLight();
	~DynamicLight();

	void onTick(Event& ev);
};