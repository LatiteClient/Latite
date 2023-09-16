#include "pch.h"
#include "BreakIndicator.h"

BreakIndicator::BreakIndicator() : TextModule("BreakProgress", "Break Progress", "Shows the break progress of the block you're breaking.", HUD, 400.f, 0, true) {
    addSetting("last", "Last Progress", "Whether to save the progress after breaking is interrupted or not", this->last);
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

    wss << std::round(prog * 100.f);
    return wss;
}
