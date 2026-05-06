#include "pch.h"
#include "ClientInstance.h"

#include "util/Util.h"

SDK::ClientInstance* SDK::ClientInstance::instance = nullptr;

SDK::ClientInstance* SDK::ClientInstance::get() {
    if (!instance) {
        // IMinecraftGame
        MinecraftGame** mcgame = reinterpret_cast<MinecraftGame**>(Signatures::Misc::minecraftGamePointer.result);
        if (!*mcgame) {
            return nullptr;
        }

        instance = (*mcgame)->getPrimaryClientInstance();
    }
    return instance;
}

SDK::BlockSource* SDK::ClientInstance::getRegion() {
    return memory::callVirtual<BlockSource*>(this, 0x1E);
}


SDK::LocalPlayer* SDK::ClientInstance::getLocalPlayer() {
    return memory::callVirtual<LocalPlayer*>(this, 0x1F);
}

SDK::GuiData* SDK::ClientInstance::getGuiData() {
    return hat::member_at<GuiData*>(this, 0x648);
}

SDK::Options* SDK::ClientInstance::getOptions() {
    return memory::callVirtual<Options*>(this, 0xB1);
}

/*SDK::ClientHMDState* SDK::ClientInstance::getClientHMDState() {
    return memory::callVirtual<ClientHMDState*>(this, 0x197);
}*/

void SDK::ClientInstance::grabCursor() {
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_grabCursor.result)(this);
}

//vtable call:
//48 8b 80 ? ? ? ? 48 8b ce ff 15 ? ? ? ? 84 db
void SDK::ClientInstance::releaseCursor() {
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_releaseCursor.result)(this);
}