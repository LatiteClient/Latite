#pragma once
#include <cstdint>

namespace memory {
	extern uintptr_t instructionToAddress(uintptr_t addy, size_t lenOperation = 3);
	extern int instructionToOffset(uintptr_t addy, size_t lenOperation = 3);
	template <typename TRet, typename... TArgs>
	extern TRet callVirtual(void* thisptr, size_t index, TArgs... argList) {
		using TFunc = TRet(__fastcall*)(void*, TArgs...);
		TFunc* vtable = *reinterpret_cast<TFunc**>(thisptr);
		return vtable[index](thisptr, argList...);
	}
}