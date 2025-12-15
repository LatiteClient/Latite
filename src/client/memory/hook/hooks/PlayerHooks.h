#pragma once
#include "../Hook.h"

class PlayerHooks : public HookGroup {
private:
	static void hkActorAttack(SDK::Actor* obj, SDK::Actor* target, void* cause, void* a4);
	static std::string* hkActorGetFormattedNameTag(SDK::LocalPlayer* obj, std::string* result);
public:
	void init(SDK::LocalPlayer* lp);

	PlayerHooks();
};