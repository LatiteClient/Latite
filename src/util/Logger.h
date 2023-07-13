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
        logInternal(Level::Info, std::format(fmt, std::forward<Args>(args)...));
	}

    template<typename... Args>
    extern inline void warn(std::string_view fmt, Args&&... args) {
        logInternal(Level::Warn, std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    extern inline void fatal(std::string_view fmt, Args&&... args) {
        logInternal(Level::Fatal, std::format(fmt, std::forward<Args>(args)...));
    }
}