#include "MaterialPtr.h"
#include "util/Util.h"
#if 0
#include "mnemosyne/Scanner.h"

SDK::MaterialPtr* SDK::MaterialPtr::getUIColor()
{
    static auto ptr = util::InstructionToAddress(mnem::ScanSignature("48 8b 05 ? ? ? ? 48 85 c0 74 ? 48 83 78"_sig));
    return reinterpret_cast<MaterialPtr*>(ptr);
}

#endif