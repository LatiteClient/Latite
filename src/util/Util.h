#pragma once

#include <string>
#include <filesystem>
#include "api/scanner/Scanner.h"

namespace util {
	extern std::filesystem::path getRoamingPath();
	extern std::filesystem::path getLatitePath();
	extern std::wstring strToWstr(std::string const& s);
	extern std::string wstrToStr(std::wstring const& ws);
	extern uintptr_t findSignature(const char* signature);

	template <typename TreatAs, typename Pointer>
	extern TreatAs& directAccess(Pointer ptr, size_t offset) {
		return *reinterpret_cast<TreatAs*>(reinterpret_cast<uintptr_t>(ptr) + offset);
	}
}
