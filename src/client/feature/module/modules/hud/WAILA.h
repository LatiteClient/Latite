#pragma once
#include "../../HUDModule.h"

namespace SDK {
	class Actor;
	class HitResult;
}

class WAILA final : public HUDModule {
public:
	WAILA();
	~WAILA() override = default;

	void render(DrawUtil& dc, bool isDefault, bool inEditor) override;
	void afterLoadConfig() override;

private:
	enum class TargetType {
		Block,
		Entity,
	};

	struct TargetInfo {
		TargetType type = TargetType::Block;
		std::wstring title;
		std::wstring detail;
		d2d::Color swatch = d2d::Colors::WHITE;
		float health = -1.f;
		float distance = 0.f;
	};

	ValueType showBlocks = BoolValue(true);
	ValueType showEntities = BoolValue(true);
	ValueType showNamespace = BoolValue(true);
	ValueType showCoordinates = BoolValue(false);
	ValueType showDistance = BoolValue(false);
	ValueType showHealth = BoolValue(true);
	ValueType entityDistance = FloatValue(6.f);
	ValueType textSize = FloatValue(18.f);
	ValueType backgroundColor = ColorValue(0.f, 0.f, 0.f, 0.68f);
	ValueType borderColor = ColorValue(0.31f, 0.24f, 0.72f, 1.f);
	ValueType titleColor = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType detailColor = ColorValue(0.45f, 0.39f, 1.f, 1.f);
	ValueType radius = FloatValue(2.f);

	std::optional<TargetInfo> getTargetInfo(bool preview);
	std::optional<TargetInfo> getBlockTarget(SDK::HitResult* hit);
	SDK::Actor* getEntityTarget(SDK::HitResult* hit, float& distance);
	std::optional<TargetInfo> getEntityInfo(SDK::Actor* actor, float distance);

	void drawHealthPips(DrawUtil& dc, float x, float y, float health);
};
