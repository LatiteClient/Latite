#pragma once

#include "../../Module.h"
#include "sdk/common/client/renderer/ItemRenderer.h"
#include "sdk/common/world/actor/ActorRenderData.h"

class ItemPhysics : public Module {
    uint32_t origPosRel = 0;
    float* newPosRel = nullptr;
    bool initialized = false;
    
    void onRenderGame(Event&);
    
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

    std::shared_ptr<Hook> glm_rotateHook = nullptr;
    std::shared_ptr<Hook> ItemRenderer_renderHook = nullptr;

    static void __fastcall glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z);
    static void __fastcall ItemRenderer_render(SDK::ItemRenderer* _this, SDK::BaseActorRenderContext* renderContext, SDK::ActorRenderData* actorRenderData);

    void onEnable() override;
    void onDisable() override;
};
