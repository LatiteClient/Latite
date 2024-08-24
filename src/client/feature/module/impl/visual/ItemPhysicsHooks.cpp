#include "pch.h"
#include "ItemPhysics.h"
#include "sdk/common/world/actor/ItemActor.h"

void __fastcall ItemPhysics::glm_rotate(glm::mat4x4& mat, float angle, float x, float y, float z) {
    static auto rotateSig = Signatures::glm_rotate.result;
    using glm_rotate_t = void(__fastcall*)(glm::mat4x4&, float, float, float, float);
    static auto glm_rotate = reinterpret_cast<glm_rotate_t>(rotateSig);

    static auto physMod = reinterpret_cast<ItemPhysics*>(Latite::getModuleManager().find("ItemPhysics").get());

    if (physMod->renderData == nullptr)
        return;

    auto curr = reinterpret_cast<SDK::ItemActor*>(physMod->renderData->actor);

    static float height = 0.5f;

    if (!physMod->actorData.contains(curr)) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> dist(0, 1);
        std::uniform_int_distribution<> dist2(0, 359);

        const auto vec = Vec3(dist2(gen), dist2(gen), dist2(gen));
        const auto sign = Vec3i(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);

        auto def = std::tuple{curr->isOnGround() ? 0.f : height, vec, sign};
        physMod->actorData.emplace(curr, def);
    }

    const float deltaTime = 1.f / static_cast<float>(Latite::get().getTimings().getFPS());

    float& yMod = std::get<0>(physMod->actorData.at(curr));

    yMod -= height * deltaTime;

    if (yMod <= 0.f)
        yMod = 0.f;

    Vec3 pos = physMod->renderData->position;
    pos.y += yMod;

    auto& vec = std::get<1>(physMod->actorData.at(curr));
    auto& sign = std::get<2>(physMod->actorData.at(curr));

    if (!curr->isOnGround() || yMod > 0.f) {
        vec.x += static_cast<float>(sign.x) * deltaTime * std::get<FloatValue>(physMod->speed) * std::get<FloatValue>(physMod->xMul);
        vec.y += static_cast<float>(sign.y) * deltaTime * std::get<FloatValue>(physMod->speed) * std::get<FloatValue>(physMod->yMul);
        vec.z += static_cast<float>(sign.z) * deltaTime * std::get<FloatValue>(physMod->speed) * std::get<FloatValue>(physMod->zMul);

        if (vec.x > 360.f)
            vec.x -= 360.f;

        if (vec.x < 0.f)
            vec.x += 360.f;

        if (vec.y > 360.f)
            vec.y -= 360.f;

        if (vec.y < 0.f)
            vec.y += 360.f;

        if (vec.z > 360.f)
            vec.z -= 360.f;

        if (vec.z < 0.f)
            vec.z += 360.f;
    }

    Vec3 renderVec = vec;

    if (curr->isOnGround() && yMod == 0.f && !std::get<BoolValue>(physMod->preserveRotations) && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
        if (std::get<BoolValue>(physMod->smoothRotations) && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
            vec.x += static_cast<float>(sign.x) * deltaTime * std::get<FloatValue>(physMod->speed) * std::get<FloatValue>(physMod->xMul);

            if (curr->stack.block != nullptr) {
                vec.z += static_cast<float>(sign.z) * deltaTime * std::get<FloatValue>(physMod->speed) * std::get<FloatValue>(physMod->zMul);

                if (vec.x > 360.f || vec.x < 0.f) {
                    vec.x = 0.f;
                    sign.x = 0;
                }

                if (vec.z > 360.f || vec.z < 0.f) {
                    vec.z = 0.f;
                    sign.z = 0;
                }
            }
            else {
                vec.y += static_cast<float>(sign.y) * deltaTime * std::get<FloatValue>(physMod->speed) * std::get<FloatValue>(physMod->yMul);

                if (vec.x - 90.f > 360.f || vec.x - 90.f < 0.f) {
                    vec.x = 90.f;
                    sign.x = 0;
                }

                if (vec.y > 360.f || vec.y < 0.f) {
                    vec.y = 0.f;
                    sign.y = 0;
                }
            }
        }

        if (!std::get<BoolValue>(physMod->smoothRotations)) {
            if (curr->stack.block != nullptr) {
                renderVec.x = 0.f;
                renderVec.z = 0.f;
            }
            else {
                renderVec.x = 90.f;
                renderVec.y = 0.f;
            }
        }
    }

    mat = translate(mat, {pos.x, pos.y, pos.z});

    glm_rotate(mat, renderVec.x, 1.f, 0.f, 0.f);
    glm_rotate(mat, renderVec.y, 0.f, 1.f, 0.f);
    glm_rotate(mat, renderVec.z, 0.f, 0.f, 1.f);
}

void ItemPhysics::ItemRenderer_render(SDK::ItemRenderer* _this, SDK::BaseActorRenderContext* renderContext, SDK::ActorRenderData* actorRenderData) {
    static auto physMod = reinterpret_cast<ItemPhysics*>(Latite::getModuleManager().find("ItemPhysics").get());

    physMod->renderData = actorRenderData;
    
    physMod->ItemRenderer_renderHook->oFunc<decltype(&ItemPhysics::ItemRenderer_render)>()(_this, renderContext, actorRenderData);
}
