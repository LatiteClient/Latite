#include "pch.h"
#include "MolangVariableMap.h"
#include "mc/Addresses.h"

SDK::MolangVariable* SDK::MolangVariableMap::_getOrAddMolangVariable(uint16_t index) {
    return reinterpret_cast<MolangVariable* (*)(MolangVariableMap*, uint16_t)>(
        Signatures::MolangVariableMap__getOrAddMolangVariable.result)(this, index);
}

SDK::MolangVariable* SDK::MolangVariableMap::getMolangVariable(uint64_t hash, char const* name) {
    if (!Signatures::MolangVariable__findOrAddVariableIndex.result || name == nullptr) {
        return nullptr;
    }

    const auto index = MolangVariable::_findOrAddVariableIndex(hash, name, false);
    if (index >= this->mMapFromVariableIndexToVariableArrayOffset.size()) {
        return nullptr;
    }

    const auto offset = this->mMapFromVariableIndexToVariableArrayOffset[index];
    if (offset < 0 || static_cast<size_t>(offset) >= this->mVariables.size()) {
        return nullptr;
    }

    return this->mVariables[offset].get();
}

std::optional<float> SDK::MolangVariableMap::getMolangVariableFloat(uint64_t hash, char const* name) {
    const auto variable = this->getMolangVariable(hash, name);
    if (variable == nullptr || variable->mValue.mType != MolangScriptArgType::Float) {
        return std::nullopt;
    }

    return variable->mValue.mPOD.mFloat;
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
