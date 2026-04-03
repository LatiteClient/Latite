#pragma once

#include "../Hook.h"

class PlayerHooks : public HookGroup {
private:
	static void* hkActorAttack(SDK::Actor* obj, void* ret, SDK::Actor* target, void* cause, void* a4);
public:
	PlayerHooks();
};
