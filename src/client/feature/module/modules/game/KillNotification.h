#pragma once
#include "client/feature/module/Module.h"

class KillNotification : public Module {
public:
    KillNotification();

    void onAttack(Event& evG);
    void onPacketReceive(Event& evG);
    void onTick(Event& evG);
    void playSound(int index);

private:
    EnumData sounds;
    ValueType useCustomSound = BoolValue(false);
    ValueType customSound = TextValue(L"random.orb");

    std::chrono::system_clock::time_point lastHurt {};
    std::unordered_set<uint64_t> lastRuntimeIds = {};
    bool hasHit = false;
};
