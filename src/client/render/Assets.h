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

	void loadAll();
	void unloadAll();
private:
	std::vector<Asset*> allAssets{};
};