#include "pch.h"
#include "ClientInstance.h"

#include "Platform_GameCore.h"

SDK::ClientInstance* SDK::ClientInstance::instance = nullptr;

SDK::ClientInstance* SDK::ClientInstance::get() {
    if (!instance) {
        // IMinecraftGame
        const auto mcgame = Platform_GameCore::get()->getMinecraftGame();
        if (!mcgame) {
            return nullptr;
        }

        instance = mcgame->getPrimaryClientInstance();
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
    return hat::member_at<Options*>(this, 0xC68);
}

SDK::ResourcePackManager& SDK::ClientInstance::getResourcePackManager() {
    return *memory::callVirtual<ResourcePackManager*>(this, 0x60);
}

void SDK::ClientInstance::grabCursor() {
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_grabCursor.result)(this);
}

// vtable call:
// 48 8b 80 ? ? ? ? 48 8b ce ff 15 ? ? ? ? 84 db
void SDK::ClientInstance::releaseCursor() {
    reinterpret_cast<void(__fastcall*)(void*)>(Signatures::ClientInstance_releaseCursor.result)(this);
}
