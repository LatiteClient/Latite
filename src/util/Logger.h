#pragma once
#include <string_view>
#include <format>

namespace Logger {
    enum class Level {
        Info,
        Warn,
        Fatal
    };

	extern void setup();

    extern void logInternal(Level level, std::string str);

    template<typename... Args>
    extern inline void info(std::string_view fmt, Args&&... args) {
        std::string fm = std::vformat(fmt, std::make_format_args(args...));
        logInternal(Level::Info, fm);
	}

    template<typename... Args>
    extern inline void warn(std::string_view fmt, Args&&... args) {
        std::string fm = std::vformat(fmt, std::make_format_args(args...));
        logInternal(Level::Warn, fm);
    }

    template<typename... Args>
    extern inline void fatal(std::string_view fmt, Args&&... args) {
        std::string fm = std::vformat(fmt, std::make_format_args(args...));
        logInternal(Level::Fatal, fm);
    }
}