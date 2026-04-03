#include "pch.h"
#include "PlayerHooks.h"

static std::shared_ptr<Hook> ActorAttackHook;

void* PlayerHooks::hkActorAttack(SDK::Actor* obj, void* ret, SDK::Actor* target, void* cause, void* a4) {
    if (obj == SDK::ClientInstance::get()->getLocalPlayer()) {
        AttackEvent ev{ target };
        Eventing::get().dispatch(ev);
    }

    return ActorAttackHook->oFunc<decltype(&hkActorAttack)>()(obj, ret, target, cause, a4);
}

PlayerHooks::PlayerHooks() {
    ActorAttackHook = this->addHook(Signatures::Actor_attack.result, &hkActorAttack, "Actor::attack");
}
