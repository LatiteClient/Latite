#pragma once
#include "MolangVariable.h"
#include <optional>

namespace SDK {
    class MolangVariableMap {
    public:
        std::vector<short> mMapFromVariableIndexToVariableArrayOffset;
        std::vector<std::unique_ptr<MolangVariable>> mVariables;
        bool mHasPublicVariables;
        bool mHasVariablesThatShouldSync;
        bool mHasVariablesThatShouldSave;

        MolangVariable* _getOrAddMolangVariable(uint16_t index);
        MolangVariable* getMolangVariable(uint64_t hash, char const* name);
        std::optional<float> getMolangVariableFloat(uint64_t hash, char const* name);
        void setMolangVariable(uint64_t hash, char const* name, float value);
    };
}
