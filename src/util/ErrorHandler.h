#pragma once

#ifdef LATITE_DEBUG
#include "util/ExceptionHandler.h"
#include "util/Logger.h"

int LogCrashDetails(StructuredException& ex);

#define BEGIN_ERROR_HANDLER try {

#define END_ERROR_HANDLER \
    } catch (StructuredException& ex) { \
        LogCrashDetails(ex); \
    } catch (const std::exception& e) { \
        Logger::Fatal("A C++ exception occurred: {}", e.what()); \
    } catch (...) { \
        Logger::Fatal("An unknown exception occurred."); \
    }

#else
#define BEGIN_ERROR_HANDLER
#define END_ERROR_HANDLER
#endif