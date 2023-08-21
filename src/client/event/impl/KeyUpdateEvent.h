#pragma once
#include "api/eventing/Event.h"
#include "util/FNV32.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"

class KeyUpdateEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(KeyUpdateEvent);

	[[nodiscard]] int getKey() { return key; }
	[[nodiscard]] int isDown() { return down; }
	[[nodiscard]] int inUI() { return !SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed(); }

	KeyUpdateEvent(int key, bool down) : key(key), down(down) {}
private:
	bool down;
	int key;
};