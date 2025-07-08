#pragma once
#include <util/DxUtil.h>
#include <mc/common/client/renderer/TexturePtr.h>
#include <filesystem>

class JsTexture {
public:
	JsTexture(std::wstring const& textureNameOrPath, bool gameTexture);

	[[nodiscard]] bool isMinecraftTexture() {
		return this->mcTexture.has_value();
	}

	[[nodiscard]] bool hasTexture() {
		return (mcTexture.has_value() && mcTexture->resourceLocation) || d2dTexture;
	}

	void loadMinecraft();
	void reloadMinecraft();

	std::filesystem::path tryGetRealPath(std::wstring const& oPath);

	~JsTexture() {
		// should be handled automatically
	}

	ID2D1Bitmap* getBitmap() { return d2dTexture.Get(); }
	SDK::TexturePtr* getTexture() {
		if (mcTexture.has_value()) return &mcTexture.value();
		return nullptr;
	}

private:
	bool gameTexture = false;
	std::wstring nameOrPath;
	ComPtr<ID2D1Bitmap> d2dTexture = nullptr;
	std::optional<SDK::TexturePtr> mcTexture = std::nullopt;
	bool failed = false;
};