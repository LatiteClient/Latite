#include "Signature.h"
#include "api/scanner/scanner.h"
#include "api/memory/memory.h"

uintptr_t memory::signature_store::ref(int offset)
{
	return memory::instructionToOffset(scanResult, offset);
}

uintptr_t memory::signature_store::deref(int offset)
{
	return memory::instructionToAddress(scanResult, offset);
}

void memory::signature_store::resolve()
{
	memory::findSignature(this->str.c_str(), hMod);
}
