#include "pch.h"
#include "ItemPhysics.h"

#include "sdk/common/world/actor/ItemActor.h"
#include "util/mem/buffer.h"

ItemPhysics::ItemPhysics() : Module("ItemPhysics", LocalizeString::get("client.module.itemPhysics.name"),
                                        LocalizeString::get("client.module.itemPhysics.desc"), GAME) {
    addSliderSetting("speed", LocalizeString::get("client.module.itemPhysics.speed.name"),
        LocalizeString::get("client.module.itemPhysics.speed.desc"), speed, FloatValue(3.f), FloatValue(15.f), FloatValue(0.1f));
    addSliderSetting("xMul", LocalizeString::get("client.module.itemPhysics.xMul.name"),
        LocalizeString::get("client.module.itemPhysics.xMul.desc"), xMul, FloatValue(7.f), FloatValue(30.f), FloatValue(0.1f));
    addSliderSetting("yMul", LocalizeString::get("client.module.itemPhysics.yMul.name"),
        LocalizeString::get("client.module.itemPhysics.yMul.desc"), yMul, FloatValue(7.f), FloatValue(30.f), FloatValue(0.1f));
    addSliderSetting("zMul", LocalizeString::get("client.module.itemPhysics.zMul.name"),
        LocalizeString::get("client.module.itemPhysics.zMul.desc"), zMul, FloatValue(7.f), FloatValue(30.f), FloatValue(0.1f));
    addSetting("presRots", LocalizeString::get("client.module.itemPhysics.presRots.name"),
        LocalizeString::get("client.module.itemPhysics.presRots.desc"), preserveRotations);
    addSetting("smoothRots", LocalizeString::get("client.module.itemPhysics.smoothRots.name"),
        LocalizeString::get("client.module.itemPhysics.smoothRots.desc"), smoothRotations, "presRots"_isfalse);

    listen<RenderGameEvent>(static_cast<EventListenerFunc>(&ItemPhysics::onRenderGame), false);
    listen<PreRenderItemEvent>(static_cast<EventListenerFunc>(&ItemPhysics::onPreRenderItem), false);
    listen<RenderItemRotateEvent>(static_cast<EventListenerFunc>(&ItemPhysics::onRenderItemRotate), false);
}

static char data[0x5], data2[0x5];

void ItemPhysics::onEnable() {
    initialized = true;
    
    static auto posAddr = Signatures::ItemPositionConst.result + 4;
    origPosRel = *reinterpret_cast<uint32_t*>(posAddr);

    static auto translateAddr = reinterpret_cast<void*>(Signatures::glm_translateRef.result);
    static auto translateAddr2 = reinterpret_cast<void*>(Signatures::glm_translateRef2.result);

    newPosRel = static_cast<float*>(AllocateBuffer(reinterpret_cast<void*>(posAddr)));
    *newPosRel = 0.f;

    const auto newRipRel = memory::getRipRel(posAddr, reinterpret_cast<uintptr_t>(newPosRel));

    memory::patchBytes(reinterpret_cast<void*>(posAddr), newRipRel.data(), 4);

    memory::copyBytes(translateAddr, data, 5);
    memory::copyBytes(translateAddr2, data2, 5);

    memory::nopBytes(translateAddr, 5);
    memory::nopBytes(translateAddr2, 5);
}

void ItemPhysics::onDisable() {
    if (!initialized)
        return;

    initialized = false;

    static auto posAddr = Signatures::ItemPositionConst.result + 4;
    static auto translateAddr = reinterpret_cast<void*>(Signatures::glm_translateRef.result);
    static auto translateAddr2 = reinterpret_cast<void*>(Signatures::glm_translateRef2.result);

    memory::patchBytes(reinterpret_cast<void*>(posAddr), &origPosRel, 4);
    FreeBuffer(newPosRel);

    memory::patchBytes(translateAddr, data, 5);
    memory::patchBytes(translateAddr2, data2, 5);

    actorData.clear();
}

void ItemPhysics::onRenderGame(Event&) {
    const auto lp = SDK::ClientInstance::get()->getLocalPlayer();
    
    static bool playerNull = lp == nullptr;

    if (playerNull != (lp == nullptr)) {
        playerNull = lp == nullptr;

        if (playerNull) {
            actorData.clear();
            renderData = nullptr;
        }
    }
}

void ItemPhysics::onPreRenderItem(Event& event) {
    const auto& ev = reinterpret_cast<PreRenderItemEvent&>(event);

    this->renderData = ev.getRenderData();
}

void ItemPhysics::onRenderItemRotate(Event& event) {
    auto& ev = reinterpret_cast<RenderItemRotateEvent&>(event);

    ev.setCancelled(true);

    static auto rotateSig = Signatures::glm_rotate.result;
    using glm_rotate_t = void(__fastcall*)(glm::mat4x4&, float, float, float, float);
    static auto glm_rotate = reinterpret_cast<glm_rotate_t>(rotateSig);

    if (renderData == nullptr)
        return;

    auto curr = reinterpret_cast<SDK::ItemActor*>(renderData->actor);

    static float height = 0.5f;

    if (!actorData.contains(curr)) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> dist(0, 1);
        std::uniform_int_distribution<> dist2(0, 359);

        const auto vec = Vec3(dist2(gen), dist2(gen), dist2(gen));
        const auto sign = Vec3i(dist(gen) * 2 - 1, dist(gen) * 2 - 1, dist(gen) * 2 - 1);

        auto def = std::tuple{curr->isOnGround() ? 0.f : height, vec, sign};
        actorData.emplace(curr, def);
    }

    const float deltaTime = 1.f / static_cast<float>(Latite::get().getTimings().getFPS());

    float& yMod = std::get<0>(actorData.at(curr));

    yMod -= height * deltaTime;

    float threshold = curr->stack.block != nullptr ? 0.f : -0.125f;

    if (yMod <= threshold)
        yMod = threshold;

    Vec3 pos = renderData->position;
    pos.y += yMod;

    auto& vec = std::get<1>(actorData.at(curr));
    auto& sign = std::get<2>(actorData.at(curr));

    if (!curr->isOnGround() || yMod > threshold) {
        vec.x += static_cast<float>(sign.x) * deltaTime * std::get<FloatValue>(speed) * std::get<FloatValue>(xMul);
        vec.y += static_cast<float>(sign.y) * deltaTime * std::get<FloatValue>(speed) * std::get<FloatValue>(yMul);
        vec.z += static_cast<float>(sign.z) * deltaTime * std::get<FloatValue>(speed) * std::get<FloatValue>(zMul);

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

    if (curr->isOnGround() && yMod == threshold && !std::get<BoolValue>(preserveRotations) && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
        if (std::get<BoolValue>(smoothRotations) && (sign.x != 0 || sign.y != 0 && sign.z != 0)) {
            vec.x += static_cast<float>(sign.x) * deltaTime * std::get<FloatValue>(speed) * std::get<FloatValue>(xMul);

            if (curr->stack.block != nullptr) {
                vec.z += static_cast<float>(sign.z) * deltaTime * std::get<FloatValue>(speed) * std::get<FloatValue>(zMul);

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
                vec.y += static_cast<float>(sign.y) * deltaTime * std::get<FloatValue>(speed) * std::get<FloatValue>(yMul);

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

        if (!std::get<BoolValue>(smoothRotations)) {
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

    *ev.getMatrix() = glm::translate(*ev.getMatrix(), {pos.x, pos.y, pos.z});

    glm_rotate(*ev.getMatrix(), renderVec.x, 1.f, 0.f, 0.f);
    glm_rotate(*ev.getMatrix(), renderVec.y, 0.f, 1.f, 0.f);
    glm_rotate(*ev.getMatrix(), renderVec.z, 0.f, 0.f, 1.f);
}
