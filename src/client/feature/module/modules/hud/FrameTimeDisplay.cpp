#include "pch.h"
#include "FrameTimeDisplay.h"
#include "client/Latite.h"

FrameTimeDisplay::FrameTimeDisplay() : TextModule("FrameTimeDisplay",
                                                  LocalizeString::get("client.textmodule.frameTimeDisplay.name"),
                                                  LocalizeString::get("client.textmodule.frameTimeDisplay.desc"),
                                                  HUD) {
    addSliderSetting("interval", LocalizeString::get("client.textmodule.frameTimeDisplay.interval.name"),
                     LocalizeString::get("client.textmodule.frameTimeDisplay.interval.desc"), interval, FloatValue(0.f),
                     FloatValue(1000.f), FloatValue(100.f));

    this->prefix = TextValue(L"Frame time: ");
    this->suffix = TextValue(L"ms");
}

std::wstringstream FrameTimeDisplay::text(bool isDefault, bool inEditor) {
    const std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    const long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();

    std::wstringstream ss;

    if (elapsed >= std::get<FloatValue>(interval)) {
        displayedValue = Latite::get().getTimings().getFrameTime();
        lastUpdate = now;
    }

    ss << std::fixed << std::setprecision(1) << displayedValue;

    return ss;
}
