#pragma once

#ifdef LATITE_CRASH_REPORTING
#include "util/ExceptionHandler.h"

#define LATITE_ERROR_HANDLER_CONCAT_INNER(a, b) a##b
#define LATITE_ERROR_HANDLER_CONCAT(a, b) LATITE_ERROR_HANDLER_CONCAT_INNER(a, b)

#define BEGIN_ERROR_HANDLER                                                                                    \
    DebugExceptionHandler::ErrorBoundaryScope LATITE_ERROR_HANDLER_CONCAT(latiteErrorBoundaryScope, __LINE__); \
    try {
#define END_ERROR_HANDLER                                                                \
    }                                                                                    \
    catch (const std::exception& e) {                                                    \
        LogExceptionDetails(e);                                                          \
        DebugExceptionHandler::AbortProcess();                                           \
    }                                                                                    \
    catch (...) {                                                                        \
        LogUnknownExceptionDetails("Caught unknown exception at Latite error boundary"); \
        DebugExceptionHandler::AbortProcess();                                           \
    }

#else
#define BEGIN_ERROR_HANDLER
#define END_ERROR_HANDLER
#endif
