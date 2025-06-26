#include "pch.h"
#include "PlayerHooks.h"

#include "client/event/impl/GetFormattedNameTagEvent.h"

static std::shared_ptr<Hook> ActorAttackHook;
static std::shared_ptr<Hook> ActorGetFormattedNameTag;

void PlayerHooks::hkActorAttack(SDK::Actor* obj, SDK::Actor* target, void* cause, bool a4) {
    if (obj == SDK::ClientInstance::get()->getLocalPlayer()) {
        AttackEvent ev{ target };
        Eventing::get().dispatch(ev);
    }

    ActorAttackHook->oFunc<decltype(&hkActorAttack)>()(obj, target, cause, a4);
}

std::string* PlayerHooks::hkActorGetFormattedNameTag(SDK::LocalPlayer* obj, std::string* result) {
    ActorGetFormattedNameTag->oFunc<decltype(&hkActorGetFormattedNameTag)>()(obj, result);
    if (obj == SDK::ClientInstance::get()->getLocalPlayer() && Latite::get().isMainThread()) {
        GetFormattedNameTagEvent ev{ result, obj };
        Eventing::get().dispatch(ev);
    }

    return result;
}

void PlayerHooks::init(SDK::LocalPlayer* lp) {
    uintptr_t* vtable = *reinterpret_cast<uintptr_t**>(lp);
    ActorGetFormattedNameTag = addTableSwapHook(reinterpret_cast<uintptr_t>(vtable + 33), &hkActorGetFormattedNameTag,
                                                "Actor::getFormattedNameTag");
}

PlayerHooks::PlayerHooks() {
    ActorAttackHook = this->addHook(Signatures::Actor_attack.result, &hkActorAttack, "Actor::attack");
}
