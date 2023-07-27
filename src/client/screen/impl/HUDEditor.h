#pragma once
#include "../Screen.h"

class HUDEditor : public Screen {
public:
	HUDEditor();
	void onEnable(bool ignoreAnims) override;
	void onDisable() override;
private:
	void onRender(Event& ev);

	float anim = 0.f;
};