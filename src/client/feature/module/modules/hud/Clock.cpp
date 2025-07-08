#include "pch.h"
#include "Clock.h"

Clock::Clock() : TextModule("Clock", LocalizeString::get("client.textmodule.clock.name"),
                            LocalizeString::get("client.textmodule.clock.desc"), HUD) {
    addSetting("militaryTime", LocalizeString::get("client.textmodule.clock.militaryTime.name"),
               LocalizeString::get("client.textmodule.clock.militaryTime.desc"), this->militaryTime);
    addSetting("showSeconds", LocalizeString::get("client.textmodule.clock.showSeconds.name"),
               LocalizeString::get("client.textmodule.clock.showSeconds.desc"), this->showSeconds);
    addSetting("showDate", LocalizeString::get("client.textmodule.clock.showDate.name"),
               LocalizeString::get("client.textmodule.clock.showDate.desc"), this->showDate);
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
