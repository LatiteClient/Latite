#include "pch.h"
#include "memory.h"

uintptr_t memory::instructionToAddress(uintptr_t addy, size_t lenOperation) {
	if (addy == 0) return 0;
	uintptr_t out = (uintptr_t)addy;
	int offset = *reinterpret_cast<int*>(out + lenOperation);
	out += offset + (static_cast<unsigned long long>(lenOperation) + sizeof(int));
	return out;
}

int memory::instructionToOffset(uintptr_t addy, size_t lenOperation) {
	if (addy == 0) return 0;
	uintptr_t out = (uintptr_t)addy;
	int offset = *reinterpret_cast<int*>(out + lenOperation);
	return offset;
}

void memory::nopBytes(void* dst, unsigned int size) {
	if (dst == nullptr)
		return;

	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memset(dst, 0x90, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

void memory::copyBytes(void* src, void* dst, unsigned int size) {
	if (src == nullptr || dst == nullptr)
		return;

	DWORD oldprotect;
	VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(src, size, oldprotect, &oldprotect);
}

void memory::patchBytes(void* dst, const void* src, unsigned int size) {
	if (src == nullptr || dst == nullptr)
		return;

	DWORD oldprotect;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oldprotect, &oldprotect);
}

std::array<std::byte, 4> memory::getRipRel(uintptr_t instructionAddress, uintptr_t targetAddress) {
	uintptr_t relAddress = targetAddress - (instructionAddress + 4); // 4 bytes for RIP-relative addressing
	std::array<std::byte, 4> relRipBytes{};

	for (size_t i = 0; i < 4; ++i) {
		relRipBytes[i] = static_cast<std::byte>((relAddress >> (i * 8)) & 0xFF);
	}

	return relRipBytes;
}
