#pragma once
#include "../../HUDModule.h"
#include <string>
#include <unordered_set>
#include <vector>

namespace SDK {
	class Level;
	class PlayerListEntry;
}

class TabList : public Module {
public:
	TabList();

	void onRenderLayer(Event&);
	void onRenderNameTag(Event&);
	void onTick(Event&);
	void afterLoadConfig() override;
	bool shouldHoldToToggle() override { return true; }
private:
	std::string getRowName(SDK::PlayerListEntry& entry) const;
	std::unordered_set<std::string> getActivePlayerNames(SDK::Level* level) const;
	std::vector<SDK::PlayerListEntry*> getSortedPlayerListRows(SDK::Level* level) const;
	ColorValue getColorOrDefault(ValueType const& value, ColorValue const& fallback) const;
	float getFloatOrDefault(ValueType const& value, float fallback) const;

	ValueType textSizeS = FloatValue(20.f);
	ValueType textCol = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType bgCol = ColorValue(0.f, 0.f, 0.f, 0.5f);
};

