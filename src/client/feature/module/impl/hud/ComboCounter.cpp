#include "pch.h"
#include "ComboCounter.h"
#include <sdk/common/network/packet/ActorEventPacket.h>

ComboCounter::ComboCounter() : TextModule("ComboCounter", LocalizeString::get("client.textmodule.comboCounter.name"),
                                          LocalizeString::get("client.textmodule.comboCounter.desc"), HUD) {
    prefix = TextValue(LocalizeString::get("client.textmodule.comboCounter.count.name"));

    listen<AttackEvent>((EventListenerFunc)&ComboCounter::onAttack);
    listen<PacketReceiveEvent>((EventListenerFunc)&ComboCounter::onPacketReceive);
    listen<TickEvent>((EventListenerFunc)&ComboCounter::onTick);
}

std::wstringstream ComboCounter::text(bool isDefault, bool inEditor) {
    return std::wstringstream() << combo;
}

void ComboCounter::onAttack(Event& evG) {
    auto& ev = reinterpret_cast<AttackEvent&>(evG);
    
    auto ent = ev.getActor();

    if (ent->getRuntimeID() != lastRuntimeId) {
        combo = 0;
    }

    lastRuntimeId = ent->getRuntimeID();
    lastHurt = std::chrono::system_clock::now();
    hasHit = true;
}

void ComboCounter::onPacketReceive(Event& evG) {
    auto& ev = reinterpret_cast<PacketReceiveEvent&>(evG);
    auto pkt = ev.getPacket();

    if (pkt->getID() == SDK::PacketID::ACTOR_EVENT && hasHit) {
        auto actorEvent = static_cast<SDK::ActorEventPacket*>(pkt);

        if (actorEvent->eventID == SDK::ActorEventID::HURT_ANIMATION && actorEvent->runtimeID == lastRuntimeId) {
            combo++;
            hasHit = false;
        }
    }
}

void ComboCounter::onTick(Event&) {
    auto now = std::chrono::system_clock::now();

    if (now - lastHurt > 3s) {
        lastRuntimeId = 0;
        combo = 0;
    }

    if (SDK::ClientInstance::get()->getLocalPlayer()->invulnerableTime > 8) {
        combo = 0;
    }
}
