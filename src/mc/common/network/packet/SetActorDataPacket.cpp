#include "pch.h"
#include "SetActorDataPacket.h"

bool SDK::SetActorDataPacket::tryGetNameTag(uint64_t* runtimeId, std::string* nameTag) const {
    if (!runtimeId || !nameTag) return false;

    constexpr uint16_t nameTagMetadataId = 4;
    constexpr uint8_t stringMetadataType = 4;

    auto packetBase = reinterpret_cast<uintptr_t>(this);
    const auto packetRuntimeId = *reinterpret_cast<const uint64_t*>(packetBase + 0x30);
    auto* metadataBegin = *reinterpret_cast<void***>(packetBase + 0x38);
    auto* metadataEnd = *reinterpret_cast<void***>(packetBase + 0x40);
    if (!metadataBegin || !metadataEnd || metadataEnd < metadataBegin) return false;

    for (auto* it = metadataBegin; it != metadataEnd; ++it) {
        auto* metadata = *it;
        if (!metadata) continue;

        auto** vtable = *reinterpret_cast<void***>(metadata);
        if (!vtable) continue;

        const auto metadataId = reinterpret_cast<uint16_t(__fastcall*)(void*)>(vtable[1])(metadata);
        const auto metadataType = reinterpret_cast<uint8_t(__fastcall*)(void*)>(vtable[2])(metadata);
        if (metadataId != nameTagMetadataId || metadataType != stringMetadataType) continue;

        *runtimeId = packetRuntimeId;
        *nameTag = *reinterpret_cast<std::string*>(reinterpret_cast<uintptr_t>(metadata) + 0x10);
        return true;
    }

    return false;
}
