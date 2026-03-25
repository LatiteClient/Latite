#include "pch.h"
#include "Minecraft.h"
#include "mc/common/client/game/ClientInstance.h"
#include "mc/common/client/player/LocalPlayer.h"

SDK::Level* SDK::Minecraft::getLevel() {
	if (gameSession) {
		return reinterpret_cast<Level*>(gameSession->level);
	}
	return nullptr;
}
