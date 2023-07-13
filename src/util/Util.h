#pragma once
#include <string>

namespace util {
	extern std::wstring getRoamingPath();
	extern std::wstring getLatitePath();
	extern std::wstring strToWstr(std::string const& s);
	extern std::string wstrToStr(std::wstring const& ws);
}