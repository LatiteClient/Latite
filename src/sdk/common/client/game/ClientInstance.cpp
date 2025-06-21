#include "pch.h"
#include "ClientInstance.h"

#include "ClientHMDState.h"
#include "util/Util.h"

SDK::ClientInstance* SDK::ClientInstance::instance = nullptr;

SDK::ClientInstance* SDK::ClientInstance::get() {
    if (!instance) {
        if (Signatures::Misc::clientInstance.result) {
            static auto sig = Signatures::Misc::clientInstance.result;
            uintptr_t evalPtr = *reinterpret_cast<uintptr_t*>(sig);
            if (!evalPtr) return nullptr;
            evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr);

            int offs = 0x58;
            if (SDK::internalVers == SDK::V1_19_51) offs = 0x48;

            evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr + offs);
            if (SDK::internalVers > SDK::V1_19_51) instance = *reinterpret_cast<ClientInstance**>(evalPtr);
            else instance = reinterpret_cast<ClientInstance*>(evalPtr);
        }
        else {
            // IMinecraftGame
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
    if (SDK::internalVers < SDK::V1_21_40) {
        return memory::callVirtual<BlockSource*>(this, 0x1C);
    }
    if (SDK::internalVers < SDK::V1_21_80) {
        return memory::callVirtual<BlockSource*>(this, 0x1D);
    }
    return memory::callVirtual<BlockSource*>(this, 0x1E);
}


SDK::LocalPlayer* SDK::ClientInstance::getLocalPlayer() {
    if (SDK::internalVers == SDK::V1_18_12 || SDK::internalVers == SDK::V1_19_51) {
        return memory::callVirtual<LocalPlayer*>(this, 0x18);
    }
    if (SDK::internalVers < SDK::V1_20_50) {
        return memory::callVirtual<LocalPlayer*>(this, 0x1B);
    }
    if (SDK::internalVers < SDK::V1_21_40) {
        return memory::callVirtual<LocalPlayer*>(this, 0x1D);
    }
    if (SDK::internalVers < SDK::V1_21_80) {
        return memory::callVirtual<LocalPlayer*>(this, 0x1E);
    }
    return memory::callVirtual<LocalPlayer*>(this, 0x1F);
}

SDK::GuiData* SDK::ClientInstance::getGuiData() {
    switch (internalVers) {
    case V1_18_12:
        return memory::callVirtual<GuiData*>(this, 0xCE);
    case V1_19_51:
        return util::directAccess<GuiData*>(this, 0x500);
    default:
        if(internalVers < V1_21_30)
        {
            return util::directAccess<GuiData*>(this, 0x558);
        }
        if (internalVers >= V1_21_80) {
            return util::directAccess<GuiData*>(this, 0x5B8);
        }
        if (internalVers >= V1_21_70) {
            return util::directAccess<GuiData*>(this, 0x5B0);
        }
        if (internalVers >= V1_21_40) {
            return util::directAccess<GuiData*>(this, 0x590);
        }
        return util::directAccess<GuiData*>(this, 0x588);
    }
    return nullptr;
}

SDK::Options* SDK::ClientInstance::getOptions() {
    return memory::callVirtual<Options*>(this, 0xC5);
}

SDK::ClientHMDState* SDK::ClientInstance::getClientHMDState() {
    return memory::callVirtual<ClientHMDState*>(this, 0x194);
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