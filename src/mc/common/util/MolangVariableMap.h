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
	};
}