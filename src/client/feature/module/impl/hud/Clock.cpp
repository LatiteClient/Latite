#include "pch.h"
#include "Clock.h"

Clock::Clock() : TextModule("Clock", "Clock", "Shows the current time.", HUD) {
    addSetting("militaryTime", "24-hour time", "Display the time in 24-hour format.", this->militaryTime);
    addSetting("showSeconds", "Show seconds", "Display time with seconds", this->showSeconds);
    addSetting("showDate", "Show date", "Display the current date along with the time", this->showDate);
}

std::string Clock::getTimeString() {
    std::time_t t = std::time(0);
    std::tm now;
    localtime_s(&now, &t);

    std::string time;
    std::string date;

    if (std::get<BoolValue>(this->militaryTime)) {
        if (std::get<BoolValue>(this->showSeconds)) {
            time = std::format("{:02}:{:02}:{:02}", now.tm_hour, now.tm_min, now.tm_sec);
        } else {
            time = std::format("{:02}:{:02}", now.tm_hour, now.tm_min);
        }
    }
    else {
        if (std::get<BoolValue>(this->showSeconds)) {
            time = std::format("{}:{:02}:{:02} {}", (now.tm_hour + 11) % 12 + 1, now.tm_min, now.tm_sec, now.tm_hour < 12 ? "AM" : "PM");
        } else {
            time = std::format("{}:{:02} {}", (now.tm_hour + 11) % 12 + 1, now.tm_min, now.tm_hour < 12 ? "AM" : "PM");
        }
    }

    if (std::get<BoolValue>(this->showDate)) {
        // maybe I'll make this date compatible with people outside good ol 'murica soon - plextora
        date = std::format("{}/{}/{}", now.tm_mon + 1, now.tm_mday, now.tm_year + 1900);
        return std::format("{} {}", date, time);
    }

    return time;
}

std::wstringstream Clock::text(bool isDefault, bool inEditor)
{
    std::wstring time = util::StrToWStr(Clock::getTimeString());
    return std::wstringstream(time);
}
