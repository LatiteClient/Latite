#include "pch.h"
#include "Clock.h"
#include "client/Latite.h"

Clock::Clock() : TextModule("Clock", "Clock", "Shows the current time.", HUD) {
    addSetting("militaryTime", "24-hour time", "Display the time in 24-hour format.", this->militaryTime);
}

std::wstringstream Clock::text(bool isDefault, bool inEditor)
{
    std::wstringstream wss;
    std::time_t t = std::time(0);
    std::tm now;
    localtime_s(&now, &t);
    if (std::get<BoolValue>(this->militaryTime)) {
    	if (now.tm_hour < 10) wss << "0";
        wss << now.tm_hour;
    }
    else {
        wss << ((now.tm_hour + 11) % 12 + 1);
    }

    wss
        << ":"
        << (now.tm_min < 10 ? "0" : "")
        << now.tm_min;

    if (!std::get<BoolValue>(this->militaryTime)) {
        wss << (now.tm_hour < 12 ? " AM" : " PM");
    }

    return wss;
}
