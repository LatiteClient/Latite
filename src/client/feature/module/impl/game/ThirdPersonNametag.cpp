#include "ThirdPersonNametag.h"
#include <sdk/signature/storage.h>

static constexpr size_t instructionSize = 6;
static char originalBytes[instructionSize] = {};
static void* instructionPointer = nullptr;

void ThirdPersonNametag::onEnable() {
    instructionPointer = reinterpret_cast<void*>(Signatures::Misc::thirdPersonNametag.result);

    if (instructionPointer) {
        memcpy(originalBytes, instructionPointer, instructionSize);

        DWORD protect;
        VirtualProtect(instructionPointer, instructionSize, PAGE_EXECUTE_READWRITE, &protect);
        memset(instructionPointer, 0x90 /*No-Operation*/, instructionSize);
        VirtualProtect(instructionPointer, instructionSize, protect, &protect);
    }
}

void ThirdPersonNametag::onDisable() {

    if (instructionPointer) {
        DWORD protect;
        VirtualProtect(instructionPointer, instructionSize, PAGE_EXECUTE_READWRITE, &protect);
        memcpy(instructionPointer, originalBytes, instructionSize);
        VirtualProtect(instructionPointer, instructionSize, protect, &protect);
    }
}