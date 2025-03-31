#include "pch.h"
#include "Options.h"

bool SDK::Options::IsGfxVSyncEnabled() {
	wchar_t userProfile[MAX_PATH];
	DWORD pathLen = GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH);
	if (pathLen == 0 || pathLen >= MAX_PATH) {
		return true;
	}

	std::wstring optionsPath(userProfile);
	optionsPath +=
		L"\\AppData\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\LocalState\\games\\com.mojang\\minecraftpe\\options.txt";

	std::ifstream file(optionsPath.c_str());
	if (!file.is_open()) {
		return true;
	}

	std::string line;
	while (std::getline(file, line)) {
		std::erase(line, '\r');
		if (line.find("gfx_vsync:") == 0) {
			return line != "gfx_vsync:0";
		}
	}

	// default to enabled
	return true;
}