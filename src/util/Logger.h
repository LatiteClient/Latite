#pragma once
#include <string_view>

namespace Logger {
	extern void setup();
	extern void log(std::string_view fmt, ...);
}