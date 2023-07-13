#pragma once
#include <string>
#include <filesystem>

namespace util {
	extern std::filesystem::path getRoamingPath();
	extern std::filesystem::path getLatitePath();
	extern std::wstring strToWstr(std::string const& s);
	extern std::string wstrToStr(std::wstring const& ws);
}