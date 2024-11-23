#include "pch.h"
#include "MinecraftGame.h"
#include "sdk/signature/storage.h"

bool SDK::MinecraftGame::isCursorGrabbed() {
    if (SDK::internalVers >= V1_20_30)
        return util::directAccess<bool>(this, Signatures::Offset::MinecraftGame_cursorGrabbed.result);
    // v1_18_12
    if (SDK::internalVers == V1_19_51) return util::directAccess<bool>(this, 0xD8);
    return util::directAccess<bool>(this, 0x318);
}

SDK::ClientInstance* SDK::MinecraftGame::getPrimaryClientInstance() {
    if (SDK::internalVers >= V1_21_40) {
        const auto map = reinterpret_cast<std::map<unsigned char, std::shared_ptr<SDK::ClientInstance>>*>(reinterpret_cast<uintptr_t>(this) + 0x828);
        return map->at(0).get();
    }
    //19AAAB62768
    // actually a NonOwnerPointer
    std::shared_ptr<ClientInstance*> inst = nullptr;
    reinterpret_cast<void* (*)(MinecraftGame*, std::shared_ptr<ClientInstance*>&)>(Signatures::MinecraftGame_getPrimaryClientInstance.result)((MinecraftGame*)((uintptr_t)this + 0x48), inst);
    if (inst) return *inst;
    return nullptr;
}
