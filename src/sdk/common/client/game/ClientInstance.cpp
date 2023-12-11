#include "pch.h"
#include "ClientInstance.h"
#include "util/Util.h"

SDK::ClientInstance* SDK::ClientInstance::instance = nullptr;

SDK::ClientInstance* SDK::ClientInstance::get() {
    if (!instance) {
        if (Signatures::Misc::clientInstance.result) {
            static auto sig = Signatures::Misc::clientInstance.result;
            uintptr_t evalPtr = *reinterpret_cast<uintptr_t*>(sig);
            if (!evalPtr) return nullptr;
            evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr);

#ifdef LATITE_BETA
            int offs = Latite::get().cInstOffs + 0x10;
            if (SDK::internalVers == V1_19_51) {
                offs = Latite::get().cInstOffs2 + 0x10;
            }
#else
            int offs = 0x58;
            if (SDK::internalVers == SDK::V1_19_51) offs = 0x48;
#endif

            evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr + offs);
            if (SDK::internalVers > SDK::V1_19_51) instance = *reinterpret_cast<ClientInstance**>(evalPtr);
            else instance = reinterpret_cast<ClientInstance*>(evalPtr);
        }
        else {
            MinecraftGame** mcgame = reinterpret_cast<MinecraftGame**>(Signatures::Misc::minecraftGamePointer.result);
            if (!*mcgame) {
                return nullptr;
            }

            instance = (*mcgame)->getPrimaryClientInstance();
        }
    }
    return instance;
}

SDK::BlockSource* SDK::ClientInstance::getRegion() {
    if (SDK::internalVers == SDK::V1_18_12 || SDK::internalVers == SDK::V1_19_51) {
        return memory::callVirtual<BlockSource*>(this, 0x17);
    }
    if (SDK::internalVers < SDK::V1_20_50) {
        return memory::callVirtual<BlockSource*>(this, 0x1A);
    }
    return memory::callVirtual<BlockSource*>(this, 0x1B);
}


SDK::LocalPlayer* SDK::ClientInstance::getLocalPlayer() {
#ifdef LATITE_BETA
    int offs = Latite::get().plrOffs;
    if (SDK::internalVers <= V1_19_51) {
        offs = Latite::get().plrOffs2;
    }

    if (SDK::internalVers >= SDK::V1_20_50) {
        offs++; // 0x1C instead of 0x1B
    }

    return memory::callVirtual<LocalPlayer*>(this, offs);
#else
    if (SDK::internalVers == SDK::V1_18_12 || SDK::internalVers == SDK::V1_19_51) {
        return memory::callVirtual<LocalPlayer*>(this, 0x18);
    }
    if (SDK::internalVers < SDK::V1_20_50) {
        return memory::callVirtual<LocalPlayer*>(this, 0x1B);
    }
    return memory::callVirtual<LocalPlayer*>(this, 0x1C);
#endif
}

SDK::GuiData* SDK::ClientInstance::getGuiData() {
    switch (internalVers) {
    case VLATEST:
        return util::directAccess<GuiData*>(this, 0x560);
    case V1_18_12:
        return memory::callVirtual<GuiData*>(this, 0xCE);
    case V1_19_51:
        return util::directAccess<GuiData*>(this, 0x500);
    }
    return nullptr;
}

void SDK::ClientInstance::grabCursor() {
    if (internalVers == V1_18_12) {
        // 1.18.12
        memory::callVirtual<void>(this, 0x116);
        return;
    }
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_grabCursor.result)(this);
}

//vtable call:
//48 8b 80 ? ? ? ? 48 8b ce ff 15 ? ? ? ? 84 db
void SDK::ClientInstance::releaseCursor() {
    if (internalVers == V1_18_12) {
        // 1.18.12
        memory::callVirtual<void>(this, 0x117);
        return;
    }
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_releaseCursor.result)(this);
}
