#include "pch.h"
#include "KillNotification.h"

#include "mc/common/network/packet/ActorEventPacket.h"

KillNotification::KillNotification()
    : Module("KillNotification", LocalizeString::get("client.module.killNotification.name"),
             LocalizeString::get("client.module.killNotification.desc"), GAME, nokeybind) {
    sounds.addEntry(EnumEntry(0, LocalizeString::get("client.module.killNotification.sound.orb.name"),
                              LocalizeString::get("client.module.killNotification.sound.orb.desc")));
    sounds.addEntry(EnumEntry(1, LocalizeString::get("client.module.killNotification.sound.explosion.name"),
                              LocalizeString::get("client.module.killNotification.sound.explosion.desc")));
    sounds.addEntry(EnumEntry(2, LocalizeString::get("client.module.killNotification.sound.screenshot.name"),
                              LocalizeString::get("client.module.killNotification.sound.screenshot.desc")));
    sounds.addEntry(EnumEntry(3, LocalizeString::get("client.module.killNotification.sound.pillagerDeath.name"),
                              LocalizeString::get("client.module.killNotification.sound.pillagerDeath.desc")));

    addEnumSetting("sound", LocalizeString::get("client.module.killNotification.sound.name"),
                   LocalizeString::get("client.module.killNotification.sound.desc"), sounds, "useCustomSound"_isfalse);
    addSetting("useCustomSound", LocalizeString::get("client.module.killNotification.useCustomSound.name"),
               LocalizeString::get("client.module.killNotification.useCustomSound.desc"), useCustomSound);
    addSetting("customSound", LocalizeString::get("client.module.killNotification.customSound.name"),
               LocalizeString::get("client.module.killNotification.customSound.desc"), customSound,
               "useCustomSound"_istrue);

    this->listen<AttackEvent>(&KillNotification::onAttack);
    this->listen<PacketReceiveEvent>(&KillNotification::onPacketReceive);
    this->listen<TickEvent>(&KillNotification::onTick);
}

void KillNotification::onAttack(Event& evG) {
    auto& ev = reinterpret_cast<AttackEvent&>(evG);
    auto* attackedActor = ev.getActor();

    lastRuntimeIds.insert(attackedActor->getRuntimeID());
    lastHurt = std::chrono::system_clock::now();
    hasHit = true;
}

void KillNotification::onPacketReceive(Event& evG) {
    auto& ev = reinterpret_cast<PacketReceiveEvent&>(evG);
    auto* pkt = ev.getPacket();

    if (pkt->getID() == SDK::PacketID::ACTOR_EVENT && hasHit) {
        auto actorEvent = static_cast<SDK::ActorEventPacket*>(pkt);

        if (actorEvent->eventID == SDK::ActorEventID::DEATH_ANIMATION) {
            if (lastRuntimeIds.erase(actorEvent->runtimeID) > 0) {
                if (std::get<BoolValue>(useCustomSound)) {
                    util::PlaySoundUI(util::WStrToStr(std::get<TextValue>(customSound).str));
                } else {
                    KillNotification::playSound(sounds.getSelectedKey());
                }
                hasHit = !lastRuntimeIds.empty();
            }
        }
    }
}

void KillNotification::onTick(Event&) {
    auto now = std::chrono::system_clock::now();

    if (now - lastHurt > 10s) {
        lastRuntimeIds.clear();
        hasHit = false;
    }
}

void KillNotification::playSound(int index) {
    switch (index) {
    case 0:
        util::PlaySoundUI("random.orb");
        break;
    case 1:
        util::PlaySoundUI("random.explode");
        break;
    case 2:
        util::PlaySoundUI("random.screenshot");
        break;
    case 3:
        util::PlaySoundUI("mob.pillager.death");
        break;
    default:
        util::PlaySoundUI("random.orb");
        break;
    }
}
