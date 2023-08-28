#include "pch.h"
#include "AppPlatform.h"
#include "util/Util.h"
#include "api/memory/memory.h"

SDK::AppPlatform* SDK::AppPlatform::get() {
    //static uintptr_t ins = util::FindSignature("48 8b 0d ? ? ? ? 4c 89 7c 24 ? c6 44 24 ? ? 48 89 4c 24 ? 48 89 5c 24");
    return nullptr;// reinterpret_cast<AppPlatform*>(memory::instructionToAddress(ins));
}
