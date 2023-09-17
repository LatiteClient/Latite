#pragma once
#include "client/feature/module/HUDModule.h"

class HealthWarning : public Module {
public:
	HealthWarning();
private:
	void onRenderLayer(Event& ev);
	ValueType vignetteColor = ColorValue(1.f, 0.f, 0.f, 0.2f);
};

