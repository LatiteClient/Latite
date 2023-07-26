#pragma once

#include <string>
#include <filesystem>
#include "api/scanner/Scanner.h"
#include "LMath.h"

namespace util {
	extern std::filesystem::path GetRootPath();
	extern std::filesystem::path GetRoamingPath();
	extern std::filesystem::path GetLatitePath();
	extern std::wstring StrToWStr(std::string const& s);
	extern std::string WStrToStr(std::wstring const& ws);
	extern std::string Format(std::string const& s);
	
	extern std::string ToLower(std::string const& s);
	extern std::string ToUpper(std::string const& s);
	extern std::vector<std::string> SplitString(std::string const& s, char delim);

	extern Color LerpColorState(Color const& current, Color const& on, Color const& off, bool state, float speed = 3.f);

	extern inline uintptr_t FindSignature(std::string_view signature) {
		return memory::findSignature(signature, "Minecraft.Windows.exe");
	}


}
