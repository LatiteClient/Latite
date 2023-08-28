#include "JsEntity.h"
#include "pch.h"

bool JsEntity::validate() {
    if (runtimeId == 1 && SDK::ClientInstance::get()->getLocalPlayer()) {
        return true;
    }
    return getEntity();
}

SDK::Actor* JsEntity::getEntity() {
    if (runtimeId == 1) return SDK::ClientInstance::get()->getLocalPlayer();
    for (auto& actor : SDK::ClientInstance::get()->minecraft->getLevel()->getRuntimeActorList()) {
        if (actor->getRuntimeID() == runtimeId) {
            return actor;
        }
    }
    return nullptr;
}

JsEntity::JsEntity(int64_t runtimeId, AccessLevel level) : runtimeId(runtimeId), level(level) {
}
