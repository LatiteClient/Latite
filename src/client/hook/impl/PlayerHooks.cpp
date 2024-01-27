#include "pch.h"
#include "PlayerHooks.h"

static std::shared_ptr<Hook> GameModeAttackHook;

void PlayerHooks::hkGameModeAttack(SDK::GameMode* obj, SDK::Actor* entity) {

	if (obj->plr == SDK::ClientInstance::get()->getLocalPlayer()) {
		AttackEvent ev{ entity };
		Eventing::get().dispatch(ev);
	}

	GameModeAttackHook->oFunc<decltype(&hkGameModeAttack)>()(obj, entity);
}

void PlayerHooks::init(SDK::LocalPlayer* lp) {
	//uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(lp);
	//uintptr_t* gmTable = *reinterpret_cast<uintptr_t**>(lp->gameMode);
	//
	//GameModeAttackHook = this->addHook(gmTable[14], &hkGameModeAttack, "GameMode::attack");
	//GameModeAttackHook->enable();
}

PlayerHooks::PlayerHooks() {
}
