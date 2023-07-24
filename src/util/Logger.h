#pragma once
#include <string_view>
#include <format>
#include <filesystem>

namespace Logger {
    enum class Level {
        Info,
        Warn,
        Fatal
    };

	extern void Setup();

    extern std::filesystem::path GetLogPath();
    extern void LogInternal(Level level, std::string str);

    template<typename... Args>
    extern inline void Info(std::string_view fmt, Args&&... args) {
        std::string fm = std::vformat(fmt, std::make_format_args(args...));
        LogInternal(Level::Info, fm);
	}

    template<typename... Args>
    extern inline void Warn(std::string_view fmt, Args&&... args) {
        std::string fm = std::vformat(fmt, std::make_format_args(args...));
        LogInternal(Level::Warn, fm);
    }

    template<typename... Args>
    extern inline void Fatal(std::string_view fmt, Args&&... args) {
        std::string fm = std::vformat(fmt, std::make_format_args(args...));
        LogInternal(Level::Fatal, fm);
    }
}