#pragma once

#include "client/resource/Resource.h"
#include "util/DxUtil.h"

class Asset {
public:
	explicit Asset(const Resource resource) : resource(resource) {}

	Asset(Asset&) = delete;
	Asset(Asset&&) = delete;
	~Asset() = default;

	void load(IWICImagingFactory* factory, ID2D1DeviceContext* dc);
	void unload();
	[[nodiscard]] ID2D1Bitmap* getBitmap() const { return bitmap.Get(); }

protected:
	Resource resource;
	ComPtr<ID2D1Bitmap> bitmap;
};