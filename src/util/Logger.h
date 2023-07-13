#pragma once
#include <string_view>

namespace Logger {
	extern void log(std::string_view fmt, ... args);
}