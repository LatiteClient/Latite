#include "pch.h"
#include "GameCore.h"

SDK::GameCore* SDK::GameCore::get() {
    if (!Signatures::Misc::gameCorePointer.result) return nullptr;
    static auto result = *reinterpret_cast<GameCore**>(Signatures::Misc::gameCorePointer.result);
    return result;
}