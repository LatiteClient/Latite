#pragma once

#include "pch.h"

namespace memory {
	extern uintptr_t scanSignature(const char* signature, LPCSTR module);
}