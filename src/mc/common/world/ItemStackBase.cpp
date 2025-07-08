#include "pch.h"
#include "ItemStackBase.h"

std::string SDK::ItemStackBase::getHoverName() {
    return "";
    std::string out;
    reinterpret_cast<std::string*(__fastcall*)(ItemStackBase*, std::string*)>(Signatures::ItemStackBase_getHoverName.result)(this, &out);
    return out;
}
