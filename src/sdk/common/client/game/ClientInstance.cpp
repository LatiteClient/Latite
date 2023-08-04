#include "ClientInstance.h"
#include "util/Util.h"
#include "api/memory/memory.h"
#include "sdk/signature/storage.h"

sdk::ClientInstance* sdk::ClientInstance::instance = nullptr;

sdk::ClientInstance* sdk::ClientInstance::get() {
    if (!instance) {
        static auto sig = Signatures::Misc::clientInstance.result;
        uintptr_t evalPtr = *reinterpret_cast<uintptr_t*>(sig);
        if (!evalPtr) return nullptr;
        evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr);

        int offs = 0x58;
        if (sdk::internalVers == sdk::V1_19_51) offs = 0x48;

        evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr + offs);
        if (sdk::internalVers > sdk::V1_19_51) instance = *reinterpret_cast<ClientInstance**>(evalPtr);
        else instance = reinterpret_cast<ClientInstance*>(evalPtr);
    }
    return instance;
}

sdk::GuiData* sdk::ClientInstance::getGuiData() {
    switch (internalVers) {
    case VLATEST:
        return util::directAccess<GuiData*>(this, 0x540);
    case V1_18_12:
        return memory::callVirtual<GuiData*>(this, 0xCE);
    case V1_19_51:
        return util::directAccess<GuiData*>(this, 0x500);
    }
    return nullptr;
}

sdk::LocalPlayer* sdk::ClientInstance::getLocalPlayer() {
    if (sdk::internalVers == sdk::V1_18_12 || sdk::internalVers == sdk::V1_19_51) {
        return memory::callVirtual<LocalPlayer*>(this, 0x18);
    }
    return memory::callVirtual<LocalPlayer*>(this, 0x1B);
}

void sdk::ClientInstance::grabCursor() {
    if (internalVers == V1_18_12) {
        // 1.18.12
        memory::callVirtual<void>(this, 0x116);
        return;
    }
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_grabCursor.result)(this);
}

void sdk::ClientInstance::releaseCursor() {
    if (internalVers == V1_18_12) {
        // 1.18.12
        memory::callVirtual<void>(this, 0x117);
        return;
    }
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_releaseCursor.result)(this);
}
