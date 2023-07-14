#include "ClientInstance.h"
#include "util/util.h"

sdk::ClientInstance* sdk::ClientInstance::get()
{
    static ClientInstance* inst = nullptr;
    if (!inst) {
        static auto sig = util::findSignature("48 8b 0d ? ? ? ? 48 85 c9 74 ? 48 83 39 ? 74 ? 48 8b 05 ? ? ? ? 48 85 c0 74 ? f0 ff 40 ? 48 8b 05 ? ? ? ? 48 8b 0d ? ? ? ? 48 89 43 ? 48 8b c3 48 89 3b c6 43 ? ? 48 89 4b ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 33 c0 48 8b cf 48 89 03 88 43 ? 48 89 43 ? 48 89 43 ? e8 ? ? ? ? 48 8b c3 48 8b 5c 24 ? 48 83 c4 ? 5f c3 cc cc cc cc cc cc cc cc cc cc cc e9 ? ? ? ? cc cc cc cc cc cc cc cc cc cc cc 48 89 5c 24 ? 48 89 74 24");
        uintptr_t evalPtr = *reinterpret_cast<uintptr_t*>(sig);
        if (!evalPtr) return nullptr;
        evalPtr = *reinterpret_cast<uintptr_t*>(evalPtr + 0x58);
        uintptr_t evalPtr = *reinterpret_cast<uintptr_t*>(sig);
        inst = *reinterpret_cast<ClientInstance**>(evalPtr);
    }
    return inst;
}
