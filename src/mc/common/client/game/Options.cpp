#include "pch.h"
#include "Options.h"
#include "mc/Addresses.h"

void SDK::Options::setPlayerViewPerspective(int perspective) {
    reinterpret_cast<void(__fastcall*)(Options*, int)>(Signatures::Options_setPerspective.result)(this, perspective);
}

int SDK::Options::getPlayerViewPerspective() {
    return reinterpret_cast<int(__fastcall*)(Options*)>(Signatures::Options_getPerspective.result)(this);
}
