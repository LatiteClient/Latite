#include "Signature.h"
#include "api/scanner/scanner.h"
#include "api/memory/memory.h"

uintptr_t memory::signature_store::ref(int offset) {
	return memory::instructionToOffset(scan_result, offset);
}

uintptr_t memory::signature_store::deref(int offset) {
	return memory::instructionToAddress(scan_result, offset);
}

bool memory::signature_store::resolve() {
	scan_result = memory::findSignature(this->signature, mod);
	if (!scan_result) return false;
	result = on_resolve(*this, scan_result);
	return true;
}
