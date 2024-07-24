#include "pch.h"
#include "BreakIndicator.h"

BreakIndicator::BreakIndicator() : TextModule("BreakProgress",
                                              LocalizeString::get("client.textmodule.breakIndicator.name"),
                                              LocalizeString::get("client.textmodule.breakIndicator.desc"), HUD, 400.f,
                                              0, true) {
    addSetting("last", LocalizeString::get("client.textmodule.breakIndicator.lastProgress.name"),
               LocalizeString::get("client.textmodule.breakIndicator.lastProgress.desc"), this->last);
}

BreakIndicator::~BreakIndicator() {
}

std::wstringstream BreakIndicator::text(bool isDefault, bool inEditor) {
    std::wstringstream wss;
    float prog = 0.f;
    auto lp = SDK::ClientInstance::get()->getLocalPlayer();
    if (lp) {
        prog = std::get<BoolValue>(last) ? lp->gameMode->lastBreakProgress : lp->gameMode->breakProgress;
    }

    wss << std::round(prog * 100.f) << L'%';
    return wss;
}
