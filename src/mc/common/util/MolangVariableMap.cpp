#include "pch.h"
#include "MolangVariableMap.h"
#include "mc/Addresses.h"

void SDK::MolangVariableMap::setMolangVariable(uint64_t hash, char const* name, float value) {
	if (!Signatures::MolangScriptArg_MolangScriptArg.result ||
		!Signatures::MolangScriptArg_destructor.result ||
		!Signatures::MolangVariableMap_setMolangVariable.result) {
		return;
	}

	using ctor_t = MolangScriptArg*(__fastcall*)(MolangScriptArg*, float);
	using setter_t = void(__fastcall*)(MolangVariableMap*, uint64_t, char const*, MolangScriptArg*);
	using dtor_t = void(__fastcall*)(MolangScriptArg*);

	MolangScriptArg arg{};
	reinterpret_cast<ctor_t>(Signatures::MolangScriptArg_MolangScriptArg.result)(&arg, value);
	reinterpret_cast<setter_t>(Signatures::MolangVariableMap_setMolangVariable.result)(this, hash, name, &arg);
	reinterpret_cast<dtor_t>(Signatures::MolangScriptArg_destructor.result)(&arg);
}
