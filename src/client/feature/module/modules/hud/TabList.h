#pragma once
#include "../../HUDModule.h"
#include "mc/common/client/renderer/TexturePtr.h"
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
	};

	struct CachedPlayerHeadTexture {
		PlayerHeadSkinKey skinKey;
		std::string texturePath;
		SDK::TexturePtr texture;
		bool hasSkinKey = false;
	};

	std::string getRowName(SDK::PlayerListEntry& entry) const;
	std::unordered_set<std::string> getActivePlayerNames(SDK::Level* level) const;
	std::vector<SDK::PlayerListEntry*> getSortedPlayerListRows(SDK::Level* level) const;
	void drawPlayerHead(MCDrawUtil& dc, SDK::PlayerListEntry& entry, d2d::Rect const& bounds) const;
	bool skinKeyMatches(PlayerHeadSkinKey const& key, SDK::SerializedSkinRef const& skin, SDK::SkinImage const& image) const;
	PlayerHeadSkinKey makeSkinKey(SDK::SerializedSkinRef const& skin, SDK::SkinImage const& image) const;
	ColorValue getColorOrDefault(ValueType const& value, ColorValue const& fallback) const;
	float getFloatOrDefault(ValueType const& value, float fallback) const;

	mutable std::unordered_map<std::string, CachedPlayerHeadTexture> cachedHeadTextures;
	ValueType textSizeS = FloatValue(20.f);
	ValueType textCol = ColorValue(1.f, 1.f, 1.f, 1.f);
	ValueType bgCol = ColorValue(0.f, 0.f, 0.f, 0.5f);
};

