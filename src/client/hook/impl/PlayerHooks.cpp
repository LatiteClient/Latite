#include "pch.h"
#include "PlayerHooks.h"

static std::shared_ptr<Hook> ActorAttackHook;

void PlayerHooks::hkActorAttack(SDK::Actor* obj, SDK::Actor* target, void* cause, bool a4) {

	if (obj == SDK::ClientInstance::get()->getLocalPlayer()) {
		AttackEvent ev{ target };
		Eventing::get().dispatch(ev);
	}

	ActorAttackHook->oFunc<decltype(&hkActorAttack)>()(obj, target, cause, a4);
}

void PlayerHooks::init(SDK::LocalPlayer* lp) {
	//uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(lp);
	//uintptr_t* gmTable = *reinterpret_cast<uintptr_t**>(lp->gameMode);
	
	//GameModeAttackHook = this->addTableSwapHook((uintptr_t)(gmTable + 14), &hkGameModeAttack, "GameMode::attack");
	//GameModeAttackHook->enable();
}

PlayerHooks::PlayerHooks() {
	ActorAttackHook = this->addHook(Signatures::Actor_attack.result, &hkActorAttack, "Actor::attack");
}
