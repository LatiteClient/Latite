#pragma once
#include "../../HUDModule.h"

class ItemCounter : public HUDModule {
public:
	ItemCounter();


	void render(DrawUtil& ctx, bool isDefault, bool inEditor) override;

	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return true; }
private:
	ValueType potions = BoolValue(true), crystals = BoolValue(false), totems = BoolValue(false), xpBottles = BoolValue(false), arrow = BoolValue(true);
	ValueType alwaysShow = BoolValue(true);
};