#pragma once

#include "pch.h"

namespace memory {
	extern uintptr_t findSignature(const char* signature, LPCSTR module);
}