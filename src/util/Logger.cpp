#include "pch.h"
#include "Logger.h"
#include "util/Util.h"
#include <ctime>
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"

void Logger::Setup() {
    auto path = util::GetLatitePath();
    std::filesystem::create_directory(path / "Logs");
    std::filesystem::remove(path / "Logs" / "latest.log");
}

void Logger::LogInternal(Level level, std::string str) {
    std::time_t t = std::time(0);   // get time now
    std::tm now;
    std::ostringstream oss;
    localtime_s(&now, &t);

    auto tmh = now.tm_hour;
    auto tmm = now.tm_min;
    auto tms = now.tm_sec;
    std::string hr = (tmh > 9 ? std::string("") : std::string("0")) + std::to_string(tmh);
    std::string mn = (tmm > 9 ? std::string("") : std::string("0")) + std::to_string(tmm);
    std::string sn = (tms > 9 ? std::string("") : std::string("0")) + std::to_string(tms);

    std::stringstream time;
    time << "[" << (now.tm_mon + 1) << "-" << now.tm_mday << "-" << (now.tm_year + 1900) << ", " << hr << ":" << mn << ":" << sn << "]";

    oss << std::put_time(&now, "%Y-%m-%d");

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

    std::string pref = time.str() + " [" + prefix + "] ";
    std::string mstr = pref + str + "\n";
    auto path = util::GetLatitePath();
    std::filesystem::path logPath = path / "Logs" / "latest.log";
    std::string archiveLogFileName = "LatiteRecode-" + oss.str() + ".log";
    std::filesystem::path archiveLogPath = path / "Logs" / archiveLogFileName;

    // using 2 file streams here might be bad practice but honestly
    // seems like an easier solution than opening and closing the stream
    // multiple times.
    std::ofstream ofsLogPath;
    ofsLogPath.open(logPath, std::ios::app);
    std::ofstream ofsArchiveLogPath;
    ofsArchiveLogPath.open(archiveLogPath, std::ios::app);

    if (!ofsLogPath.fail()) {
        ofsLogPath << mstr;
        ofsLogPath.close();
    }
    if (!ofsArchiveLogPath.fail()) {
        ofsArchiveLogPath << mstr;
        ofsArchiveLogPath.close();
    }
    OutputDebugStringA(mstr.c_str());

#if LATITE_DEBUG
    Latite::get().getClientMessageSink().push(util::Format("&7" + pref + "&r" + str));
#endif
}
