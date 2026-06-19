#include "pch.h"
#include "MolangVariableMap.h"
#include "mc/Addresses.h"

SDK::MolangVariable* SDK::MolangVariableMap::_getOrAddMolangVariable(uint16_t index) {
    return reinterpret_cast<MolangVariable*(*)(MolangVariableMap*, uint16_t)>(Signatures::MolangVariableMap__getOrAddMolangVariable.result)(this, index);
}

void SDK::MolangVariableMap::setMolangVariable(uint64_t hash, char const* name, float value) {
    if (!Signatures::MolangVariable__findOrAddVariableIndex.result) {
        return;
    }

    const auto index = MolangVariable::_findOrAddVariableIndex(hash, name, false);
    const auto variable = this->_getOrAddMolangVariable(index);

    if (variable != nullptr) {
        variable->mValue.mType = MolangScriptArgType::Float;
        variable->mValue.mPOD.mFloat = value;
    }
}
