#pragma once

#ifdef LATITE_DEBUG
#include "util/ExceptionHandler.h"
#include "util/Logger.h"
#include <stdexcept>

void LogExceptionDetails(StructuredException& ex);
void LogExceptionDetails(const std::exception& e);

#define BEGIN_ERROR_HANDLER try {

#define END_ERROR_HANDLER \
    } catch (StructuredException& ex) { \
        LogExceptionDetails(ex); \
    } catch (const std::exception& e) { \
        LogExceptionDetails(e); \
    } catch (...) { \
        LogExceptionDetails(std::runtime_error("An unknown exception occurred.")); \
    }

#else
#define BEGIN_ERROR_HANDLER
#define END_ERROR_HANDLER
#endif
