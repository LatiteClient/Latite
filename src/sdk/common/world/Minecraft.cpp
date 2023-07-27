#include "Minecraft.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/player/LocalPlayer.h"

sdk::Level* sdk::Minecraft::getLevel() {
	// very special case
	if (sdk::internalVers == V1_19_51) {
		if (sdk::ClientInstance::get()->getLocalPlayer()) {
			return sdk::ClientInstance::get()->getLocalPlayer()->level_1_19_51;
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
