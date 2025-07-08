#pragma once
#include <string>
#include "util/DxUtil.h"

class Asset {
public:
	Asset(std::wstring const& relPath);
	Asset(Asset&) = delete;
	Asset(Asset&&) = delete;
	~Asset() = default;

	void load(IWICImagingFactory* factory, ID2D1DeviceContext* dc);
	void unload();
	[[nodiscard]] ID2D1Bitmap* getBitmap() noexcept { return bitmap.Get(); }

protected:
	std::wstring relPath;
	ComPtr<ID2D1Bitmap> bitmap;
};