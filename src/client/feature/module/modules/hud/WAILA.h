#pragma once
#include "../../HUDModule.h"

class WAILA final : public HUDModule {
public:
	WAILA();
	~WAILA() override = default;

	void render(DrawUtil& ctx, bool isDefault, bool inEditor) override;
	void afterLoadConfig() override;

	virtual bool forceMinecraftRenderer() override { return true; }

private:
	enum class TargetType {
		Block,
		Entity,
	};

	struct TargetInfo {
		TargetType type = TargetType::Block;
		std::wstring title;
		std::wstring detail;
		SDK::Block const* block = nullptr;
		SDK::ItemStack* itemStack = nullptr;
		float health = -1.f;
	};

	ValueType showBlocks = BoolValue(true);
	ValueType showEntities = BoolValue(true);
	ValueType showNamespace = BoolValue(true);
	ValueType showCoordinates = BoolValue(false);
	ValueType showDistance = BoolValue(false);
	ValueType showHealth = BoolValue(true);
	ValueType entityDistance = FloatValue(6.f);
	ValueType textSize = FloatValue(32.f);
	ValueType backgroundColor = ColorValue(0.055f, 0.065f, 0.075f, 0.82f);
	ValueType borderColor = ColorValue(0.54f, 0.47f, 1.00f, 1.f);
	ValueType titleColor = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType detailColor = ColorValue(0.50f, 0.43f, 1.f, 1.f);
	ValueType radius = FloatValue(0.f);

	std::optional<TargetInfo> getTargetInfo(bool preview);
	std::optional<TargetInfo> getBlockTarget(SDK::HitResult* hit);
	SDK::Actor* getEntityTarget(SDK::HitResult* hit, float& distance);
	std::optional<TargetInfo> getEntityInfo(SDK::Actor* actor, float distance);

	void drawHealthPips(DrawUtil& dc, float x, float y, float health);
	void drawTargetIcon(DrawUtil& ctxGeneric, TargetInfo const& target, d2d::Rect const& icon);
};
