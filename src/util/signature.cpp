#include "pch.h"
#include "Signature.h"
#include "util/memory.h"
#include <mnemosyne/scan/scanner.hpp>

uintptr_t memory::signature_store::ref(int offset) {
	return memory::instructionToOffset(scan_result, offset);
}

uintptr_t memory::signature_store::deref(int offset) {
	return memory::instructionToAddress(scan_result, offset);
}

bool memory::signature_store::resolve() {
	if (!signature.has_value()) return true;

	auto* const scanBytes = reinterpret_cast<uint8_t*>(GetModuleHandleA(0));
	auto* const dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(GetModuleHandleA(0));
	auto* const ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(scanBytes + dosHeader->e_lfanew);
	const auto sizeOfCode = ntHeaders->OptionalHeader.SizeOfImage;

	mnem::scanner scanner{ mnem::memory_span{(std::byte*)scanBytes, (size_t)sizeOfCode} };


	scan_result = reinterpret_cast<uintptr_t>(scanner.scan_signature(this->signature.value())); //memory::findSignature(this->signature, mod);
	if (!scan_result) return false;
	result = on_resolve(*this, scan_result);
	return true;
}
