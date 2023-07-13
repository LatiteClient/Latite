#include "Logger.h"
#include "util/Util.h"
#include "pch.h"

void Logger::setup()
{
	std::wstring path = util::getLatitePath();
	std::filesystem::create_directory(path);
}

void Logger::log(std::string_view fmt, ... args)
{
	std::wstring path = util::getLatitePath();
	std::wstring logFolder = path + L"\\Logs";

    va_list args;
    va_start(args, fmt);

    std::string formatted = std::vformat(fmt, args);

    va_end(args);

    std::ofstream ofs;
    ofs.open(fmt, std::ios::app);

    if (!ofs.fail()) {
        ofs << formatted;
    }
    OutputDebugStringA(formatted.c_str());
}
