#include "pch.h"
#include "ExceptionHandler.h"

#ifdef LATITE_DEBUG

void __cdecl translate_seh_to_cpp_exception(unsigned int u, EXCEPTION_POINTERS* pExp) {
    throw StructuredException(pExp);
}

__declspec(thread) CONTEXT g_CxxExceptionContext = {};
__declspec(thread) bool g_bHasCxxExceptionContext = false;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    if (pExceptionInfo->ExceptionRecord->ExceptionCode == 0xE06D7363) { // C++ exception
        g_CxxExceptionContext = *pExceptionInfo->ContextRecord;
        g_bHasCxxExceptionContext = true;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}


#endif