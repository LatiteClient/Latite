#pragma once
#include "asset/Asset.h"
#include <vector>

class Assets final {
public:
	Assets();
	~Assets();
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
	Asset logoWhite{ L"latitewhite.png" };
	Asset document{ L"document.png" };

	void loadAll();
	void unloadAll();
private:
	std::vector<Asset*> allAssets{};
};