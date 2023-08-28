#include "pch.h"
#include "Logger.h"
#include "util/Util.h"
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
    localtime_s(&now, &t);

    auto tmh = now.tm_hour;
    auto tmm = now.tm_min;
    auto tms = now.tm_sec;
    std::string hr = (tmh > 9 ? std::string("") : std::string("0")) + std::to_string(tmh);
    std::string mn = (tmm > 9 ? std::string("") : std::string("0")) + std::to_string(tmm);
    std::string sn = (tms > 9 ? std::string("") : std::string("0")) + std::to_string(tms);

    std::stringstream time;
    time << "[" << (now.tm_mon + 1) << "-" << now.tm_mday << "-" << (now.tm_year + 1900) << ", " << hr << ":" << mn << ":" << sn << "]";

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
    auto logPath = path / "Logs" / "latest.log";

    std::ofstream ofs;
    ofs.open(logPath, std::ios::app);

    if (!ofs.fail()) {
        ofs << mstr;
    }
    OutputDebugStringA(mstr.c_str());

#if LATITE_DEBUG
    Latite::get().getClientMessageSink().push(util::Format("&7" + pref + "&r" + str));
#endif
}
