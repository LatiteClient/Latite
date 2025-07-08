#pragma once
#include "../../HUDModule.h"

class ArmorHUD : public HUDModule {
private:
	ValueType horizontal = BoolValue(false);
	ValueType dynamicColor = BoolValue(false);

	ValueType durability = BoolValue(true);
	ValueType durabilityText = BoolValue(true);
	ValueType background = BoolValue(true);

	static constexpr int mode_vetical = 0;
	static constexpr int mode_horizontal = 1;
	EnumData mode;

	ValueType showDamage = BoolValue(false);
	ValueType percentage = BoolValue(true);
	ValueType txtColor = ColorValue();
public:
	ArmorHUD();
	virtual ~ArmorHUD() = default;

	virtual void render(DrawUtil& ctx, bool isDefault, bool inEditor);

	// Force Armor Hud to use Minecraft renderer at all times
	virtual bool forceMinecraftRenderer() override { return true; }
private:
	d2d::Rect drawItem(MCDrawUtil& dc, Vec2 pos, SDK::ItemStack* stack, float size = 1.f);
};