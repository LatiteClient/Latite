#include "pch.h"
#include "ExceptionHandler.h"

#ifdef LATITE_DEBUG

void __cdecl translate_seh_to_cpp_exception(unsigned int u, EXCEPTION_POINTERS* pExp) {
    throw StructuredException(pExp);
}

#endif