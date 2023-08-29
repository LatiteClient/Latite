#include "pch.h"
#include "Clock.h"
#include <codecvt>
#include "client/Latite.h"

Clock::Clock() : TextModule("Clock", "Clock", "Shows the current time.", HUD) {
    addSetting("militaryTime", "24-hour time", "Display the time in 24-hour format.", this->militaryTime);
    addSetting("showDate", "Show date", "Display the current date along with the time", this->showDate);
}

std::string Clock::getTimeString() {
    std::time_t t = std::time(0);
    std::tm now;
    localtime_s(&now, &t);

    std::string result;

    if (std::get<BoolValue>(this->militaryTime)) {
        result += std::format("{:2}", now.tm_hour);
    }
    else {
        result += std::format("{:2}", (now.tm_hour + 11) % 12 + 1);
    }

    result += ":" + std::format("{:2}", now.tm_min);

    if (!std::get<BoolValue>(this->militaryTime)) {
        result += now.tm_hour < 12 ? " AM" : " PM";
    }

    if (std::get<BoolValue>(this->showDate)) {
        result += std::format("{}/{}/{}", now.tm_mon, now.tm_mday, now.tm_year);
    }

    return result;
}

std::wstringstream Clock::text(bool isDefault, bool inEditor)
{
    std::wstring time = util::StrToWStr(Clock::getTimeString());
    return std::wstringstream(time);
}
