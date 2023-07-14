#include "memory.h"
#include "pch.h"

uintptr_t memory::instructionToAddress(uintptr_t addy, size_t lenOperation)
{
	if (addy == 0) return 0;
	uintptr_t out = (uintptr_t)addy;
	int offset = *reinterpret_cast<int*>(out + lenOperation);
	out += offset + (static_cast<unsigned long long>(lenOperation) + sizeof(int));
	return out;
}

int memory::instructionToOffset(uintptr_t addy, size_t lenOperation)
{
	if (addy == 0) return 0;
	uintptr_t out = (uintptr_t)addy;
	int offset = *reinterpret_cast<int*>(out + lenOperation);
	return offset;
}

template<typename TRet, typename... TArgs>
TRet memory::callVirtual(void* thisptr, size_t index, TArgs... argList)
{
	return (*static_cast<TRet(void*, decltype(argList)...)**>(thisptr))[index](thisptr, argList...);
}