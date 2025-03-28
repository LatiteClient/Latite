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
	// normal caching in the TextModule constructor didnt work so im doing this
    static std::wstring cachedText;
    static std::chrono::steady_clock::time_point lastUpdate;
    static float displayedValue = -1.0f;

    const float currentValue = Latite::get().getTimings().getFrameTime();
    const std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
    const long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count();

    if (elapsed >= std::get<FloatValue>(interval)) {
        std::wstringstream ss;
        ss << std::fixed << std::setprecision(1) << currentValue;
        cachedText = ss.str();
        displayedValue = currentValue;
        lastUpdate = now;
    }

    std::wstringstream output;
    if (!cachedText.empty()) {
        output.str(cachedText);
        output.clear();
    }
    return output;
}
