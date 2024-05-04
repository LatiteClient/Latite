#pragma once
#include <client/feature/module/Module.h>

class Freelook : public Module {
public:
	Freelook();
	virtual ~Freelook() {};

	void onCameraUpdate(Event&);
	void onPerspective(Event&);

	void onEnable() override;
	void onDisable() override;
private:

	Vec2 lastRot;
	bool shouldHoldToToggle() override { return true; }
};