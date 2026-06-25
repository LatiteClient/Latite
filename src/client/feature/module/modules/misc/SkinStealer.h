#pragma once
#include "../../Module.h"

class SkinStealer : public Module {
public:
	SkinStealer();

	void onEnable() override;
	void onDisable() override;
	bool isToggleable() override { return false; }
	bool shouldPersistEnabled() override { return false; }
	void loadConfig(SettingGroup& resolvedGroup) override;
};
