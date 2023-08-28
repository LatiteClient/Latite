#include "pch.h"
#include "Minecraft.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/player/LocalPlayer.h"

SDK::Level* SDK::Minecraft::getLevel() {
	// very special case
	if (SDK::internalVers == V1_19_51) {
		if (SDK::ClientInstance::get()->getLocalPlayer()) {
			return SDK::ClientInstance::get()->getLocalPlayer()->level_1_19_51;
		}
		return nullptr;
	}

	if (gameSession) {
		auto v2 = gameSession->level;
		if (v2)
			return *v2;
	}
	return nullptr;
}
