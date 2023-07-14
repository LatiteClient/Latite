#pragma once

#include "pch.h"

namespace memory {
	extern uintptr_t ScanSignature(const char* signature, LPCSTR module);
}