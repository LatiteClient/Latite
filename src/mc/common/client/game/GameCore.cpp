#include "pch.h"
#include "GameCore.h"

#include "Platform_GameCore.h"

SDK::GameCore* SDK::GameCore::get() {
    return SDK::Platform_GameCore::get()->getGameCore();
}
