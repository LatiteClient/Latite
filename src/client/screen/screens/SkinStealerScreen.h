#pragma once
#include "../Screen.h"

#include <chrono>
#include <cstdint>
#include <optional>

namespace SDK {
	class PlayerListEntry;
	class SkinImage;
}

class SkinStealerScreen : public Screen {
public:
	SkinStealerScreen();

	std::string getName() override { return "SkinStealer"; }

	void onRender(Event& ev);
	void onClick(Event& ev);
	void onKey(Event& ev);
	void onCleanup(Event& ev);

protected:
	void onEnable(bool ignoreAnimations) override;
	void onDisable() override;

private:
	struct PlayerRow {
		std::string playerName;
		std::wstring displayName;
		std::wstring status;
		std::string skinKey;
		d2d::Rect getButtonRect = {};
		std::chrono::steady_clock::time_point feedbackUntil = {};
	};

	struct HeadBitmap {
		std::string skinKey;
		ComPtr<ID2D1Bitmap1> bitmap;
	};

	void rebuildRows();
	uint64_t getPlayerListSignature() const;
	void pruneHeadCache();
	HeadBitmap* getHeadBitmap(SDK::PlayerListEntry& entry);
	std::string makeSkinKey(SDK::PlayerListEntry& entry, SDK::SkinImage const& image) const;
	std::optional<std::filesystem::path> saveSkin(SDK::PlayerListEntry& entry);
	bool isValidSkinImage(SDK::SkinImage const *image) const;
	uint64_t hashSkin(SDK::SkinImage const &image, std::string const &id) const;
	void openSkinStealerFolder();
	std::filesystem::path makeSkinOutputPath(std::string const& playerName, SDK::SkinImage const& image) const;
	std::string sanitizeFileName(std::string name) const;
	void updateScrollbarDrag(Vec2 const& mouse);

	std::vector<uint8_t> makePlayerHeadRgba(SDK::SkinImage const& image) const;
	bool writeRgbaPng(std::filesystem::path const& path, SDK::SkinImage const& image) const;

	std::vector<PlayerRow> rows;
	std::unordered_map<std::string, HeadBitmap> headBitmaps;
	d2d::Rect panelRect = {};
	d2d::Rect listRect = {};
	d2d::Rect closeButtonRect = {};
	d2d::Rect folderButtonRect = {};
	d2d::Rect scrollbarTrackRect = {};
	d2d::Rect scrollbarThumbRect = {};
	float scroll = 0.f;
	float scrollMax = 0.f;
	float lerpScroll = 0.f;
	float scrollbarDragOffset = 0.f;
	uint64_t playerListSignature = 0;
	bool rowsDirty = true;
	bool draggingScrollbar = false;
};
