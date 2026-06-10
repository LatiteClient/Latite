#pragma once
#include "../../HUDModule.h"
#include "../src/mc/common/client/renderer/texture/TextureUVCoordinateSet.h"

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
		SDK::Actor* actor = nullptr;
		SDK::ItemStack* itemStack = nullptr;
		std::string faceTexturePath;
		Vec2 faceUvPos{};
		Vec2 faceUvSize{ 1.f, 1.f };
		float health = -1.f;
		float maxHealth = -1.f;
	};

	ValueType showBlocks = BoolValue(true);
	ValueType showEntities = BoolValue(true);
	ValueType showNamespace = BoolValue(true);
	ValueType showCoordinates = BoolValue(false);
	ValueType showDistance = BoolValue(false);
	ValueType showHealth = BoolValue(true);
	ValueType entityDistance = FloatValue(6.f);
	ValueType textSize = FloatValue(36.f);
	ValueType titleColor = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType detailColor = ColorValue(0.33f, 0.33f, 1.00f, 1.f);

	std::optional<TargetInfo> getTargetInfo(bool preview);
	std::optional<TargetInfo> getBlockTarget(SDK::HitResult* hit);
	SDK::Actor* getEntityTarget(SDK::HitResult* hit, float& distance);
	std::optional<TargetInfo> getEntityInfo(SDK::Actor* actor, float distance);
	std::string getPlayerFaceTexturePath(SDK::Player* player);

	void drawHealthHearts(DrawUtil& dc, float x, float y, float health, float maxHealth);
	void drawTargetIcon(DrawUtil& ctxGeneric, TargetInfo const& target, d2d::Rect const& icon);
};
