#pragma once
#include "MolangVariable.h"

namespace SDK {
	class MolangVariableMap {
	public:
		std::vector<short> mMapFromVariableIndexToVariableArrayOffset;
		std::vector<std::unique_ptr<MolangVariable>> mVariables;
		bool mHasPublicVariables;
		bool mHasVariablesThatShouldSync;
		bool mHasVariablesThatShouldSave;

	    MolangVariable* _getOrAddMolangVariable(uint16_t index);
		void setMolangVariable(uint64_t hash, char const* name, float value);
	};
}
