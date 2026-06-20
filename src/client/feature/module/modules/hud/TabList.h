#pragma once
#include "../../HUDModule.h"
#include "mc/common/client/renderer/TexturePtr.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace SDK {
	class Level;
	class PlayerListEntry;
	class SerializedSkinRef;
	class SkinImage;
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
	struct PlayerHeadSkinKey {
		std::string id;
		uint32_t width = 0;
		uint32_t height = 0;
		uintptr_t bytes = 0;
		size_t byteCount = 0;
		uint64_t sampleHash = 0;
	};

	struct CachedPlayerHeadTexture {
		PlayerHeadSkinKey skinKey;
		std::string texturePath;
		SDK::TexturePtr texture;
		std::chrono::steady_clock::time_point nextPathResolveAttempt {};
		std::chrono::steady_clock::time_point nextTextureLoadAttempt {};
		uint32_t pathResolveAttempts = 0;
		uint32_t textureLoadAttempts = 0;
		bool hasSkinKey = false;
	};

	struct CachedPlayerRow {
		SDK::PlayerListEntry* entry = nullptr;
		std::string playerName;
		std::string displayName;
		std::string strippedName;
		std::wstring displayNameWide;
		std::wstring strippedNameWide;
		float measuredWidth = 0.f;
		int colorSortIndex = 16;
	};

	std::string getRowName(SDK::PlayerListEntry& entry) const;
	bool refreshActivePlayerNames(SDK::Level* level);
	void syncNameTagCache(SDK::Level* level);
	void rebuildRows(SDK::Level* level);
	void updateLayoutMeasurements(MCDrawUtil& dc, std::wstring const& title, float textSize, float rowTextOffset);
	void drawPlayerHead(MCDrawUtil& dc, SDK::PlayerListEntry& entry, d2d::Rect const& bounds) const;
	void pruneHeadTextureCache();
	bool skinKeyMatches(PlayerHeadSkinKey const& key, SDK::SerializedSkinRef const& skin, SDK::SkinImage const& image) const;
	PlayerHeadSkinKey makeSkinKey(SDK::SerializedSkinRef const& skin, SDK::SkinImage const& image) const;
	uint64_t getSkinSampleHash(SDK::SkinImage const& image) const;
	ColorValue getColorOrDefault(ValueType const& value, ColorValue const& fallback) const;
	float getFloatOrDefault(ValueType const& value, float fallback) const;

	mutable std::unordered_map<std::string, CachedPlayerHeadTexture> cachedHeadTextures;
	std::unordered_set<std::string> cachedActivePlayerNames;
	std::vector<CachedPlayerRow> cachedRows;
	std::wstring cachedLayoutTitle;
	float cachedLongestText = 0.f;
	float cachedLayoutTextSize = 0.f;
	float cachedLayoutRowTextOffset = 0.f;
	uint64_t cachedNetworkNameTagsRevision = 0;
	bool rowsDirty = true;
	bool layoutDirty = true;

	ValueType textSizeS = FloatValue(20.f);
	ValueType textCol = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType bgCol = ColorValue(0.f, 0.f, 0.f, 0.5f);
};

