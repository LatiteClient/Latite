#pragma once
#include "../Hook.h"

class PlayerHooks : public HookGroup {
private:
	static void hkActorAttack(SDK::Actor* obj, SDK::Actor* target, void* cause, bool a4);
public:
	void init(SDK::LocalPlayer* lp);

	PlayerHooks();
};