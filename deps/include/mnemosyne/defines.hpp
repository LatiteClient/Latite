#pragma once

#ifdef _WIN32
#define MNEMOSYNE_OS_WINDOWS
#endif

#ifdef _MSC_VER
#define MNEMOSYNE_ABI_MICROSOFT
#else
#define MNEMOSYNE_ABI_ITANIUM
#endif