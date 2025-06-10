#pragma once

#ifdef LATITE_DEBUG
#include <windows.h>

int LogCrashDetails(struct _EXCEPTION_POINTERS* exceptionInfo);

#define BEGIN_ERROR_HANDLER __try {

#define END_ERROR_HANDLER \
    } __except(LogCrashDetails(GetExceptionInformation())) { \
    }

#else
#define BEGIN_ERROR_HANDLER
#define END_ERROR_HANDLER
#endif