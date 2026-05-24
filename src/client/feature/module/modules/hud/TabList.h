#pragma once
#include "../../HUDModule.h"
#include <unordered_map>

class TabList : public Module {
public:
	TabList();

	void onRenderOverlay(Event&);
	void onRenderNameTag(Event&);
	void onTick(Event&);
	void afterLoadConfig() override;
	bool shouldHoldToToggle() override { return true; }
private:
	std::unordered_map<std::string, std::string> coloredNameCache;
	ValueType textSizeS = FloatValue(20.f);
	ValueType textCol = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType bgCol = ColorValue(0.f, 0.f, 0.f, 0.5f);
};

