#pragma once

#include "../../Module.h"
#include "sdk/common/world/actor/ActorRenderData.h"

class ItemPhysics : public Module {
    uint32_t origPosRel = 0;
    float* newPosRel = nullptr;
    bool initialized = false;
    
    void onRenderGame(Event&);
    void onPreRenderItem(Event& event);
    void onRenderItemRotate(Event& event);
    
public:
    ItemPhysics();

    ValueType speed = FloatValue(8.f);
    ValueType xMul = FloatValue(18.f);
    ValueType yMul = FloatValue(16.f);
    ValueType zMul = FloatValue(18.f);
    ValueType preserveRotations = BoolValue(false);
    ValueType smoothRotations = BoolValue(true);
    
    std::unordered_map<SDK::Actor*, std::tuple<float, Vec3, Vec3i>> actorData;
    SDK::ActorRenderData* renderData = nullptr;

    void onEnable() override;
    void onDisable() override;
};
