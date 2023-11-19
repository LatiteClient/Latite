#include "pch.h"
#include "MaterialPtr.h"
#include "util/Util.h"

SDK::MaterialPtr* SDK::MaterialPtr::getUIColor() {
    return reinterpret_cast<SDK::MaterialPtr*>(Signatures::Misc::uiColorMaterial.result);
}

SDK::MaterialPtr* SDK::MaterialPtr::getUITextureAndColor() {
    return reinterpret_cast<SDK::MaterialPtr*>(Signatures::Misc::uiColorMaterial.result + 0x20);
}

