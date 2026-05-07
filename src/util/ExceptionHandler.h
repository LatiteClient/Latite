#pragma once

#include <exception>
#include <filesystem>
#include <string>
#include <string_view>
#include <windows.h>
#include <eh.h>

#ifdef LATITE_DEBUG

// A custom exception class to wrap SEH exceptions
class StructuredException {
private:
    EXCEPTION_RECORD m_exceptionRecord = {};
    CONTEXT m_contextRecord = {};
    EXCEPTION_POINTERS m_exceptionPointers = {};
public:
    StructuredException(EXCEPTION_POINTERS* exceptionPointers) {
        if (exceptionPointers) {
            if (exceptionPointers->ExceptionRecord) {
                m_exceptionRecord = *exceptionPointers->ExceptionRecord;
            }
            if (exceptionPointers->ContextRecord) {
                m_contextRecord = *exceptionPointers->ContextRecord;
            }
        }

        m_exceptionPointers.ExceptionRecord = &m_exceptionRecord;
        m_exceptionPointers.ContextRecord = &m_contextRecord;
    }

    EXCEPTION_POINTERS* getExceptionPointers() { return &m_exceptionPointers; }
};

void __cdecl translate_seh_to_cpp_exception(unsigned int u, EXCEPTION_POINTERS* pExp);

extern __declspec(thread) CONTEXT g_CxxExceptionContext;
extern __declspec(thread) bool g_bHasCxxExceptionContext;
LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo);

namespace DebugExceptionHandler {
    void Install();
    void Uninstall();
    bool IsHandlingCrash();
    std::string GenerateStackTrace(CONTEXT* contextArg = nullptr);
    std::filesystem::path WriteCrashReport(EXCEPTION_POINTERS* exceptionInfo, std::string_view reason);
}

void LogExceptionDetails(StructuredException& ex);
void LogExceptionDetails(const std::exception& e);

#endif
