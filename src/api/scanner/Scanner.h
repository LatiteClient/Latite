#pragma once
#include <cstdint>
#include <string_view>

namespace memory {
	extern uintptr_t findSignature(std::string_view signature, const char* module);
}