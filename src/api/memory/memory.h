#pragma once
#include <cstdint>

namespace memory {
	extern uintptr_t instructionToAddress(uintptr_t addy, size_t lenOperation = 3);
	extern int instructionToOffset(uintptr_t addy, size_t lenOperation = 3);
}