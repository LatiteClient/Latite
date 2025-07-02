#include "pch.h"
#include "Logger.h"
#include "util/Util.h"
#include <ctime>

#include "client/Latite.h"
#include "client/misc/ClientMessageQueue.h"

#ifdef LATITE_DEBUG
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#include <string>
#include <iomanip>
#include <sstream>

std::string GenerateStackTrace(CONTEXT* contextArg = nullptr) {
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

    if (!SymInitialize(process, searchPath.c_str(), FALSE)) {
        Logger::Warn("GenerateStackTrace: SymInitialize failed with error code: {}", GetLastError());
        return "Stack trace generation failed: SymInitialize error.";
    }

    DWORD64 baseAddress = (DWORD64)hModule;

    MODULEINFO moduleInfo = { 0 };
    DWORD dllSize = 0;
    if (GetModuleInformation(process, hModule, &moduleInfo, sizeof(moduleInfo))) {
        dllSize = moduleInfo.SizeOfImage;
    }

    std::string regularPdbPath = (std::filesystem::path(searchPath) / "LatiteRewrite.pdb").string();
    if (!SymLoadModule64(process, NULL, regularPdbPath.c_str(), NULL, baseAddress, dllSize)) {
        Logger::Warn(
            "Could not find or load PDB at {}. Stack trace will not have symbols. System error code: {}",
            regularPdbPath, GetLastError());
    }

    SymRefreshModuleList(process);

    CONTEXT context;
    if (contextArg) {
        context = *contextArg;
    } else {
        RtlCaptureContext(&context);
    }

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    std::string stackTraceStr = "\n--- Stack Trace ---\n";

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    while (StackWalk64(machineType, process, thread, &stackFrame, &context, NULL, SymFunctionTableAccess64,
        SymGetModuleBase64, NULL)) {
        std::string frameStr = "";
        DWORD64 displacement = 0;

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, &displacement, symbol)) {
            frameStr += "at " + std::string(symbol->Name);

            IMAGEHLP_LINE64 line;
            memset(&line, 0, sizeof(IMAGEHLP_LINE64));
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            DWORD lineDisplacement = 0;
            if (SymGetLineFromAddr64(process, stackFrame.AddrPC.Offset, &lineDisplacement, &line)) {
                frameStr += " in " + std::string(line.FileName) + ":" + std::to_string(line.LineNumber);
            }
        }
        else {
            std::stringstream ss;
            ss << "0x" << std::hex << std::setfill('0') << std::setw(8) << stackFrame.AddrPC.Offset;
            frameStr += "at [unknown function] (" + ss.str() + ")";
        }
        stackTraceStr += frameStr + "\n";
    }

    SymCleanup(process);
    return stackTraceStr;
}

void LogExceptionDetails(StructuredException& ex) {
    EXCEPTION_POINTERS* exceptionInfo = ex.getExceptionPointers();

    Logger::Fatal("An unrecoverable SEH exception occurred.");
    Logger::Fatal("Exception Code: {:#x}", exceptionInfo->ExceptionRecord->ExceptionCode);
    Logger::Fatal("Exception Address: {:#x}", (DWORD64)exceptionInfo->ExceptionRecord->ExceptionAddress);

    std::string stackTrace = GenerateStackTrace(exceptionInfo->ContextRecord);
    Logger::Fatal(stackTrace);

    // scuffed way to hope the file is written before termination
    Sleep(1000);
}

void LogExceptionDetails(const std::exception& e) {
    Logger::Fatal("An unrecoverable C++ exception occurred: {}", e.what());

    if (g_bHasCxxExceptionContext) {
        std::string stackTrace = GenerateStackTrace(&g_CxxExceptionContext);
        Logger::Fatal(stackTrace);
        g_bHasCxxExceptionContext = false;
    } else {
        std::string stackTrace = GenerateStackTrace();
        Logger::Fatal(stackTrace);
    }

    Sleep(1000);
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
