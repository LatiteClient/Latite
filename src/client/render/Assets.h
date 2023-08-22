/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "asset/Asset.h"
#include <vector>

class Assets final {
public:
	Assets();
	Assets(Assets&) = delete;
	Assets(Assets&&) = delete;

	Asset latiteLogo{ L"logo.png" };
	Asset searchIcon{ L"searchicon.png" };
	Asset arrowIcon{ L"arrow.png" };
	Asset xIcon{ L"x.png" };
	Asset hudEditIcon{ L"hudedit.png" };
	Asset arrowBackIcon{ L"arrow_back.png" };
	Asset cogIcon{ L"cog.png" };
	Asset checkmarkIcon{ L"checkmark.png" };

	void loadAll();
	void unloadAll();
private:
	std::vector<Asset*> allAssets{};
};