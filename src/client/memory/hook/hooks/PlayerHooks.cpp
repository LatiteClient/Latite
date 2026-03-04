#include "pch.h"
#include "PlayerHooks.h"

static std::shared_ptr<Hook> ActorAttackHook;

void PlayerHooks::hkActorAttack(SDK::Actor* obj, SDK::Actor* target, void* cause, void* a4) {
    if (obj == SDK::ClientInstance::get()->getLocalPlayer()) {
        AttackEvent ev{ target };
        Eventing::get().dispatch(ev);
    }

    ActorAttackHook->oFunc<decltype(&hkActorAttack)>()(obj, target, cause, a4);
}

PlayerHooks::PlayerHooks() {
    ActorAttackHook = this->addHook(Signatures::Actor_attack.result, &hkActorAttack, "Actor::attack");
}
