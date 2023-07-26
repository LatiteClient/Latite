#include "ClientInstance.h"
#include "util/util.h"
#include "api/memory/memory.h"
#include "sdk/signature/storage.h"

sdk::ClientInstance* sdk::ClientInstance::instance = nullptr;

sdk::ClientInstance* sdk::ClientInstance::get() {
    if (!instance) {
        // TODO: move this to the signature namespace
        static auto sig = Signatures::Misc::clientInstance.result;
        uintptr_t evalPtr = *reinterpret_cast<uintptr_t*>(sig);
        if (!evalPtr) return nullptr;
        evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr);
        evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr + 0x58);
        if (sdk::internalVers > sdk::V1_19_41) instance = *reinterpret_cast<ClientInstance**>(evalPtr);
        else instance = reinterpret_cast<ClientInstance*>(evalPtr);
    }
    return instance;
}

sdk::LocalPlayer* sdk::ClientInstance::getLocalPlayer() {
    if (sdk::internalVers == sdk::V1_18_12) {
        return memory::callVirtual<LocalPlayer*>(this, 0x18);
    }
    return memory::callVirtual<LocalPlayer*>(this, 0x1B);
}

void sdk::ClientInstance::grabCursor() {
    if (internalVers != VLATEST) {
        // 1.18.12
        memory::callVirtual<void>(this, 0x116);
        return;
    }
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_grabCursor.result)(this);
}

void sdk::ClientInstance::releaseCursor() {
    if (internalVers != VLATEST) {
        // 1.18.12
        memory::callVirtual<void>(this, 0x117);
        return;
    }
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_releaseCursor.result)(this);
}
