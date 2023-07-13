#include "Logger.h"
#include "util/Util.h"
#include "pch.h"

void Logger::setup()
{
	auto path = util::getLatitePath();
	std::filesystem::create_directory(path);
	std::filesystem::create_directory(path / "Logs");
}

void Logger::logInternal(Level level, std::string str)
{
    std::string prefix = "";
    switch (level) {
    case Level::Info:
        prefix = "INFO";
        break;
    case Level::Warn:
        prefix = "WARN";
        break;
    case Level::Fatal:
        prefix = "FATAL";
        break;
    }

    str = "[" + prefix + "] " + str + "\n";
    auto path = util::getLatitePath();
    auto logPath = path / "Logs" / "log.txt";

    std::ofstream ofs;
    ofs.open(logPath, std::ios::app);

    if (!ofs.fail()) {
        ofs << str;
    }
    OutputDebugStringA(str.c_str());
}
