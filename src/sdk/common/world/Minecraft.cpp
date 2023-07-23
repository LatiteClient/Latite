#include "Minecraft.h"

sdk::Level* sdk::Minecraft::getLevel() {
	if (gameSession) {
		auto v2 = gameSession->level;
		if (v2)
			return *v2;
	}
	return nullptr;
}
