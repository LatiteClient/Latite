/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

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

private:
	std::wstring relPath;
	ComPtr<ID2D1Bitmap> bitmap;
};