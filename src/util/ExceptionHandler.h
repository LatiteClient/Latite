#pragma once

#include <windows.h>
#include <eh.h>

#ifdef LATITE_DEBUG

// A custom exception class to wrap SEH exceptions
class StructuredException {
private:
    EXCEPTION_POINTERS* m_exceptionPointers;
public:
    StructuredException(EXCEPTION_POINTERS* exceptionPointers) : m_exceptionPointers(exceptionPointers) {}

    EXCEPTION_POINTERS* getExceptionPointers() const { return m_exceptionPointers; }
};

void __cdecl translate_seh_to_cpp_exception(unsigned int u, EXCEPTION_POINTERS* pExp);

extern __declspec(thread) CONTEXT g_CxxExceptionContext;
extern __declspec(thread) bool g_bHasCxxExceptionContext;
LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo);

#endif