#pragma once
#include "../Hook.h"

class PlayerHooks : public HookGroup {
private:
	static void hkGameModeAttack(SDK::GameMode* obj, SDK::Actor* entity);
public:
	void init(SDK::LocalPlayer* lp);

	PlayerHooks();
};