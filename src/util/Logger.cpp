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
    std::time_t t = std::time(0);   // get time now
    std::tm now;
    localtime_s(&now, &t);

    auto tmh = now.tm_hour + 1;
    auto tmm = now.tm_min + 1;
    std::string hr = std::to_string(tmh) + (tmh > 10 ? std::string("") : std::string("0"));
    std::string mn = std::to_string(tmm) + (tmm > 10 ? std::string("") : std::string("0"));

    std::stringstream time;
    time << "[" << (now.tm_mon + 1) << "-" << now.tm_mday << ", " << hr << ":" << mn << "]";

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

    str = time.str() + " [" + prefix + "] " + str + "\n";
    auto path = util::getLatitePath();
    auto logPath = path / "Logs" / "log.txt";

    std::ofstream ofs;
    ofs.open(logPath, std::ios::app);

    if (!ofs.fail()) {
        ofs << str;
    }
    OutputDebugStringA(str.c_str());
}
