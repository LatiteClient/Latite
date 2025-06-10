#include "pch.h"
#include "Logger.h"
#include "util/Util.h"
#include <ctime>
#include "client/Latite.h"
#include "client/misc/ClientMessageQueue.h"

#ifdef LATITE_DEBUG
#include <windows.h>
#include <dbghelp.h>
#include <string>

#include "util/ExceptionHandler.h"

std::string GenerateStackTrace(EXCEPTION_POINTERS* exceptionInfo) {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS);

    char dllPath[MAX_PATH];
    HMODULE hModule = NULL;

    GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&GenerateStackTrace,
        &hModule
    );

    GetModuleFileNameA(hModule, dllPath, sizeof(dllPath));

    std::string searchPath = std::filesystem::path(dllPath).parent_path().string();

    if (!SymInitialize(process, searchPath.c_str(), TRUE)) {
        Logger::Warn("GenerateStackTrace: SymInitialize failed with error code: {}", GetLastError());
        return "Stack trace generation failed: SymInitialize error.";
    }

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));
    DWORD machineType;
    CONTEXT* context = exceptionInfo->ContextRecord;

    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context->Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context->Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    std::string stackTraceStr = "\n--- Stack Trace ---\n";

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    while (StackWalk64(machineType, process, thread, &stackFrame, context, NULL, SymFunctionTableAccess64,
                       SymGetModuleBase64, NULL)) {
        std::string frameStr = "";

        IMAGEHLP_LINE64 line;
        memset(&line, 0, sizeof(IMAGEHLP_LINE64));
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD lineDisplacement = 0;

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        DWORD64 displacement = 0;

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol)) {
            frameStr += "at " + std::string(symbol->Name);

            if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &line)) {
                frameStr += " in " + std::string(line.FileName) + ":" + std::to_string(line.LineNumber);
            }
        }
        else {
            frameStr += "at [unknown function] (0x" + std::to_string(stackFrame.AddrPC.Offset) + ")";
        }

        stackTraceStr += frameStr + "\n";
    }

    SymCleanup(process);
    return stackTraceStr;
}

int LogCrashDetails(StructuredException& ex) {
    EXCEPTION_POINTERS* exceptionInfo = ex.getExceptionPointers();

    Logger::Fatal("Exception Code: {:#x}", exceptionInfo->ExceptionRecord->ExceptionCode);
    Logger::Fatal("Exception Address: {:#x}", (DWORD64)exceptionInfo->ExceptionRecord->ExceptionAddress);

    std::string stackTrace = GenerateStackTrace(exceptionInfo);
    Logger::Fatal(stackTrace);

    // scuffed way to hope the file is written before termination
    Sleep(1000);

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void Logger::Setup() {
    auto path = util::GetLatitePath();
    std::filesystem::create_directory(path / "Logs");
    std::filesystem::remove(path / "Logs" / "latest.log");
}

void Logger::LogInternal(Level level, std::string str) {
    std::time_t t = std::time(0); // get time now
    std::tm now;
    std::ostringstream oss;
    localtime_s(&now, &t);

    auto tmh = now.tm_hour;
    auto tmm = now.tm_min;
    auto tms = now.tm_sec;
    std::string hr = (tmh > 9 ? std::string("") : std::string("0")) + std::to_string(tmh);
    std::string mn = (tmm > 9 ? std::string("") : std::string("0")) + std::to_string(tmm);
    std::string sn = (tms > 9 ? std::string("") : std::string("0")) + std::to_string(tms);

    std::stringstream time;
    time << "[" << (now.tm_mon + 1) << "-" << now.tm_mday << "-" << (now.tm_year + 1900) << ", " << hr << ":" << mn <<
        ":" << sn << "]";

    oss << std::put_time(&now, "%Y-%m-%d");

    std::string prefix = "";
    switch (level) {
    case Level::Info:
        prefix = "INFO";
        break;
    case Level::Warn:
        prefix = "WARN";
        break;
    case Level::Fatal:
        prefix = "FATAL";
        break;
    }

    std::string pref = time.str() + " [" + prefix + "] ";
    std::string mstr = pref + str + "\n";
    auto path = util::GetLatitePath();
    std::filesystem::path logPath = path / "Logs" / "latest.log";
    std::string archiveLogFileName = "LatiteRecode-" + oss.str() + ".log";
    std::filesystem::path archiveLogPath = path / "Logs" / archiveLogFileName;

    // using 2 file streams here might be bad practice but honestly
    // seems like an easier solution than opening and closing the stream
    // multiple times.
    std::ofstream ofsLogPath;
    ofsLogPath.open(logPath, std::ios::app);
    std::ofstream ofsArchiveLogPath;
    ofsArchiveLogPath.open(archiveLogPath, std::ios::app);

    if (!ofsLogPath.fail()) {
        ofsLogPath << mstr;
        ofsLogPath.close();
    }
    if (!ofsArchiveLogPath.fail()) {
        ofsArchiveLogPath << mstr;
        ofsArchiveLogPath.close();
    }
    OutputDebugStringA(mstr.c_str());

#if LATITE_DEBUG
    Latite::get().getClientMessageQueue().push(util::Format("&7" + pref + "&r" + str));
#endif
}
