#pragma once
#include "xorstr.hpp"

#ifdef LATITE_DEBUG
#define XOR_STRING
#else
#define XOR_STRING xorstr_
#endif