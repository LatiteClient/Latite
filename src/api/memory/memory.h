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
	extern void nopBytes(void* dst, unsigned int size);
	extern void copyBytes(void* src, void* dst, unsigned int size);
	extern void patchBytes(void* dst, const void* src, unsigned int size);
	extern std::array<std::byte, 4> getRipRel(uintptr_t instructionAddress, uintptr_t targetAddress);
}