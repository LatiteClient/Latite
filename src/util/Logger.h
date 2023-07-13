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

    // TODO: Actual logging using std::format and constexpr
    template<typename... Args>
    extern constexpr inline void info(std::string fmt, Args&&... args) {
        logInternal(Level::Info, fmt);
	}

    template<typename... Args>
    extern constexpr inline void warn(std::string fmt, Args&&... args) {
       // std::string fm = std::format(fmt, std::forward<Args>(args)...);
        logInternal(Level::Warn, fmt);
    }

    template<typename... Args>
    extern constexpr inline void fatal(std::string fmt, Args&&... args) {
       // std::string fm = std::format(fmt, std::forward<Args>(args)...);
        logInternal(Level::Fatal, fmt);
    }
}