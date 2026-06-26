#pragma once
#include "../../HUDModule.h"
#include "../src/mc/common/client/renderer/texture/TextureUVCoordinateSet.h"
#include "mc/common/world/ItemTier.h"

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

    enum class ToolMode : int {
        HighestTier,
        MinimumTier,
        Fastest,
    };

    struct PanelLayout {
        float defaultWidth = 252.f;
        float defaultHeight = 81.f;
        float iconSlotSize = 54.f;
        float iconSize = 48.f;
        float iconInset = 3.f;
        float actorIconScale = 1.125f;
        float paddingX = 12.f;
        float paddingY = 12.f;
        float textGap = 9.f;
        float lineGap = 3.f;
        float titleTextScale = 0.78f;
        float detailTextScale = 0.92f;
        float frameThickness = 3.f;
    };

    struct ToolLayout {
        float iconSize = 30.f;
        float gap = 6.f;
        float spacing = 3.f;
        float speedEpsilon = 0.0001f;
    };

    struct HealthLayout {
        int heartsPerRow = 10;
        float heartSize = 18.f;
    };

    struct TargetInfo {
        TargetType type = TargetType::Block;
        std::wstring title;
        std::wstring detail;
        SDK::Block const* block = nullptr;
        SDK::Actor* actor = nullptr;
        SDK::ItemStack* itemStack = nullptr;
        std::vector<std::string> toolItemIds;
        std::string faceTexturePath;
        Vec2 faceUvPos { 0.125f, 0.125f };
        Vec2 faceUvSize { 0.125f, 0.125f };
        float health = -1.f;
        float maxHealth = -1.f;
    };

    const PanelLayout panelLayout;
    const ToolLayout toolLayout;
    const HealthLayout healthLayout;

    ValueType showBlocks = BoolValue(true);
    ValueType showEntities = BoolValue(true);
    ValueType showNamespace = BoolValue(true);
    ValueType showItemId = BoolValue(false);
    ValueType showCoordinates = BoolValue(false);
    ValueType showDistance = BoolValue(false);
    ValueType showHarvest = BoolValue(true);
    EnumData toolMode;
    ValueType showHealth = BoolValue(true);
    ValueType entityDistance = FloatValue(6.f);
    ValueType textSize = FloatValue(36.f);
    ValueType titleColor = ColorValue(1.f, 1.f, 1.f, 1.f);
    ValueType detailColor = ColorValue(0.33f, 0.33f, 1.00f, 1.f);

    std::unordered_map<SDK::Block const*, std::vector<std::string>> minimumTierToolCache;
    std::unordered_map<SDK::Block const*, std::vector<std::string>> fastestToolCache;
    std::unordered_map<SDK::Block const*, std::vector<std::string>> highestTierToolCache;

    std::optional<TargetInfo> getTargetInfo(bool preview);
    std::optional<TargetInfo> getBlockTarget(SDK::HitResult* hit);
    SDK::Actor* getEntityTarget(SDK::HitResult* hit, float& distance);
    std::optional<TargetInfo> getEntityInfo(SDK::Actor* actor, float distance);

    SDK::ItemTier const* getToolTier(SDK::Item* item, std::string_view itemId) const;
    std::vector<std::string> findPreferredToolItemIds(SDK::Block const& block, ToolMode toolMode);
    std::optional<std::wstring> getLocalizedMinecraftName(std::string const& key) const;
    std::wstring getBlockDisplayName(SDK::Block const& block, std::string const& localizationKey,
                                     std::string const& fallbackName) const;
    std::wstring titleCaseIdentifier(std::string id) const;
    Vec3 rayDirectionFromHit(SDK::HitResult* hit) const;
    std::string getPlayerFaceTexturePath(SDK::Player* player) const;
    Vec2 getHeartPosition(float x, float y, int index) const;

    void drawInspectorPanel(DrawUtil& dc, d2d::Rect const& bounds) const;
    void drawHealthHearts(DrawUtil& dc, float x, float y, float health, float maxHealth) const;
    void drawTargetIcon(DrawUtil& ctxGeneric, TargetInfo const& target, d2d::Rect const& icon) const;
};
