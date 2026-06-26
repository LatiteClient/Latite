#include "MolangVariable.h"

uint16_t SDK::MolangVariable::_findOrAddVariableIndex(uint64_t hash, const char* name, bool allowSpecialCharacters) {
    return reinterpret_cast<uint16_t (*)(uint64_t, const char*, bool)>(
        Signatures::MolangVariable__findOrAddVariableIndex.result)(hash, name, allowSpecialCharacters);
}
