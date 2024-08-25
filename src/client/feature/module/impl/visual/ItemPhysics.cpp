#include "pch.h"
#include "ItemPhysics.h"

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
