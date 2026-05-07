#include "pch.h"
#include "ExceptionHandler.h"

#ifdef LATITE_DEBUG
#include "util/Util.h"

#include <atomic>
#include <dbghelp.h>
#include <eh.h>
#include <iomanip>
#include <psapi.h>

namespace {
    constexpr DWORD cppExceptionCode = 0xE06D7363;

    PVOID vectoredHandler = nullptr;
    LPTOP_LEVEL_EXCEPTION_FILTER previousUnhandledExceptionFilter = nullptr;
    std::terminate_handler previousTerminateHandler = nullptr;

    INIT_ONCE symbolLockInit = INIT_ONCE_STATIC_INIT;
    CRITICAL_SECTION symbolLock = {};
    bool symbolsInitialized = false;
    bool ownsSymbolHandler = false;
    std::atomic_long crashHandlerEntered = 0;

    BOOL CALLBACK InitSymbolLock(PINIT_ONCE, PVOID, PVOID*) {
        InitializeCriticalSection(&symbolLock);
        return TRUE;
    }

    void EnterSymbolLock() {
        InitOnceExecuteOnce(&symbolLockInit, InitSymbolLock, nullptr, nullptr);
        EnterCriticalSection(&symbolLock);
    }

    void LeaveSymbolLock() {
        LeaveCriticalSection(&symbolLock);
    }

    std::string LastErrorToString(DWORD error) {
        return std::format("{}", error);
    }

    std::string MakeTimestamp(bool fileSafe) {
        std::time_t t = std::time(nullptr);
        std::tm now = {};
        localtime_s(&now, &t);

        std::ostringstream oss;
        if (fileSafe) {
            oss << std::put_time(&now, "%Y-%m-%d_%H-%M-%S");
        }
        else {
            oss << std::put_time(&now, "%Y-%m-%d %H:%M:%S");
        }

        return oss.str();
    }

    std::filesystem::path LogsPath() {
        return util::GetLatitePath() / "Logs";
    }

    std::filesystem::path CrashPath() {
        return LogsPath() / "Crashes";
    }

    std::filesystem::path DailyLogPath() {
        std::time_t t = std::time(nullptr);
        std::tm now = {};
        localtime_s(&now, &t);

        std::ostringstream oss;
        oss << "LatiteRecode-" << std::put_time(&now, "%Y-%m-%d") << ".log";
        return LogsPath() / oss.str();
    }

    void AppendRawLog(std::string const& text) {
        OutputDebugStringA(text.c_str());

        try {
            std::filesystem::create_directories(LogsPath());

            std::ofstream latest(LogsPath() / "latest.log", std::ios::app);
            if (!latest.fail()) {
                latest << text;
                latest.flush();
            }

            std::ofstream archive(DailyLogPath(), std::ios::app);
            if (!archive.fail()) {
                archive << text;
                archive.flush();
            }
        }
        catch (...) {
        }
    }

    HMODULE GetLatiteModule() {
        HMODULE module = nullptr;
        GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&GetLatiteModule),
            &module
        );

        return module;
    }

    std::filesystem::path GetModuleFilePath(HMODULE module) {
        char path[MAX_PATH] = {};
        if (!module || !GetModuleFileNameA(module, path, sizeof(path))) {
            return {};
        }

        return path;
    }

    void AddSymbolPathPart(std::string& symbolPath, std::filesystem::path const& path) {
        if (path.empty()) {
            return;
        }

        auto part = path.string();
        if (part.empty()) {
            return;
        }

        if (!symbolPath.empty()) {
            symbolPath += ';';
        }
        symbolPath += part;
    }

    void AddSymbolPathPart(std::string& symbolPath, char const* path) {
        if (!path || !*path) {
            return;
        }

        if (!symbolPath.empty()) {
            symbolPath += ';';
        }
        symbolPath += path;
    }

    std::string BuildSymbolPath() {
        std::string symbolPath;

        auto latiteModulePath = GetModuleFilePath(GetLatiteModule());
        AddSymbolPathPart(symbolPath, latiteModulePath.parent_path());
        AddSymbolPathPart(symbolPath, util::GetLatitePath());
        AddSymbolPathPart(symbolPath, CrashPath());

        char currentDirectory[MAX_PATH] = {};
        if (GetCurrentDirectoryA(sizeof(currentDirectory), currentDirectory)) {
            AddSymbolPathPart(symbolPath, currentDirectory);
        }

        char ntSymbolPath[32767] = {};
        if (GetEnvironmentVariableA("_NT_SYMBOL_PATH", ntSymbolPath, sizeof(ntSymbolPath))) {
            AddSymbolPathPart(symbolPath, ntSymbolPath);
        }

        char ntAltSymbolPath[32767] = {};
        if (GetEnvironmentVariableA("_NT_ALT_SYMBOL_PATH", ntAltSymbolPath, sizeof(ntAltSymbolPath))) {
            AddSymbolPathPart(symbolPath, ntAltSymbolPath);
        }

        return symbolPath;
    }

    void EnsureLatiteModuleLoaded(HANDLE process) {
        auto module = GetLatiteModule();
        auto modulePath = GetModuleFilePath(module);
        if (!module || modulePath.empty()) {
            return;
        }

        MODULEINFO moduleInfo = {};
        GetModuleInformation(process, module, &moduleInfo, sizeof(moduleInfo));

        auto baseAddress = reinterpret_cast<DWORD64>(module);
        if (SymGetModuleBase64(process, baseAddress) != 0) {
            return;
        }

        SymLoadModuleEx(
            process,
            nullptr,
            modulePath.string().c_str(),
            nullptr,
            baseAddress,
            moduleInfo.SizeOfImage,
            nullptr,
            0
        );
    }

    bool EnsureSymbolsLocked() {
        if (symbolsInitialized) {
            return true;
        }

        HANDLE process = GetCurrentProcess();
        auto symbolPath = BuildSymbolPath();

        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_DEFERRED_LOADS);

        if (SymInitialize(process, symbolPath.empty() ? nullptr : symbolPath.c_str(), TRUE)) {
            symbolsInitialized = true;
            ownsSymbolHandler = true;
            EnsureLatiteModuleLoaded(process);
            return true;
        }

        DWORD error = GetLastError();
        if (error == ERROR_INVALID_PARAMETER) {
            // DbgHelp is process-global. If something else initialized it first,
            // keep using that session and update the search path for LatiteDebug.pdb.
            SymSetSearchPath(process, symbolPath.c_str());
            symbolsInitialized = true;
            ownsSymbolHandler = false;
            EnsureLatiteModuleLoaded(process);
            return true;
        }

        AppendRawLog(std::format(
            "[{}] [FATAL] SymInitialize failed while preparing crash symbols. Error: {}\n",
            MakeTimestamp(false),
            LastErrorToString(error)
        ));
        return false;
    }

    std::string FormatAddress(DWORD64 address) {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << address;
        return oss.str();
    }

    std::string FormatModuleOffset(HANDLE process, DWORD64 address) {
        IMAGEHLP_MODULE64 moduleInfo = {};
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);

        if (SymGetModuleInfo64(process, address, &moduleInfo)) {
            std::string moduleName;
            if (moduleInfo.ImageName && *moduleInfo.ImageName) {
                moduleName = std::filesystem::path(moduleInfo.ImageName).filename().string();
            }
            else {
                moduleName = moduleInfo.ModuleName;
            }

            auto offset = address - moduleInfo.BaseOfImage;
            std::ostringstream oss;
            oss << moduleName << "+0x" << std::hex << std::uppercase << offset;
            return oss.str();
        }

        return FormatAddress(address);
    }

    std::string FormatStackFrame(HANDLE process, DWORD64 address) {
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
        auto symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;

        std::ostringstream frame;
        DWORD64 displacement = 0;

        if (SymFromAddr(process, address, &displacement, symbol)) {
            frame << FormatModuleOffset(process, address) << " ";
            frame << symbol->Name;

            if (displacement != 0) {
                frame << "+0x" << std::hex << std::uppercase << displacement << std::dec;
            }
        }
        else {
            frame << FormatModuleOffset(process, address);
        }

        IMAGEHLP_LINE64 line = {};
        line.SizeOfStruct = sizeof(line);
        DWORD lineDisplacement = 0;
        if (SymGetLineFromAddr64(process, address, &lineDisplacement, &line)) {
            frame << " in " << line.FileName << ":" << line.LineNumber;
        }

        frame << " [" << FormatAddress(address) << "]";
        return frame.str();
    }

    void InitializeStackFrame(CONTEXT const& context, STACKFRAME64& stackFrame, DWORD& machineType) {
        memset(&stackFrame, 0, sizeof(stackFrame));

#if defined(_M_X64)
        machineType = IMAGE_FILE_MACHINE_AMD64;
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrFrame.Offset = context.Rbp;
        stackFrame.AddrStack.Offset = context.Rsp;
#elif defined(_M_IX86)
        machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = context.Eip;
        stackFrame.AddrFrame.Offset = context.Ebp;
        stackFrame.AddrStack.Offset = context.Esp;
#else
#error Unsupported architecture for LatiteDebug stack traces.
#endif

        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Mode = AddrModeFlat;
    }

    std::filesystem::path WriteMiniDump(EXCEPTION_POINTERS* exceptionInfo, std::string const& baseName) {
        try {
            std::filesystem::create_directories(CrashPath());
        }
        catch (...) {
        }

        auto dumpPath = CrashPath() / (baseName + ".dmp");
        HANDLE dumpFile = CreateFileW(
            dumpPath.wstring().c_str(),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (dumpFile == INVALID_HANDLE_VALUE) {
            AppendRawLog(std::format(
                "[{}] [FATAL] Failed to create minidump {}. Error: {}\n",
                MakeTimestamp(false),
                dumpPath.string(),
                LastErrorToString(GetLastError())
            ));
            return {};
        }

        MINIDUMP_EXCEPTION_INFORMATION dumpException = {};
        dumpException.ThreadId = GetCurrentThreadId();
        dumpException.ExceptionPointers = exceptionInfo;
        dumpException.ClientPointers = FALSE;

        auto dumpType = static_cast<MINIDUMP_TYPE>(
            MiniDumpNormal |
            MiniDumpWithDataSegs |
            MiniDumpWithHandleData |
            MiniDumpWithThreadInfo |
            MiniDumpWithUnloadedModules |
            MiniDumpWithIndirectlyReferencedMemory |
            MiniDumpWithFullMemoryInfo
        );

        BOOL wroteDump = MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            dumpFile,
            dumpType,
            exceptionInfo ? &dumpException : nullptr,
            nullptr,
            nullptr
        );

        DWORD error = wroteDump ? ERROR_SUCCESS : GetLastError();
        FlushFileBuffers(dumpFile);
        CloseHandle(dumpFile);

        if (!wroteDump) {
            AppendRawLog(std::format(
                "[{}] [FATAL] MiniDumpWriteDump failed for {}. Error: {}\n",
                MakeTimestamp(false),
                dumpPath.string(),
                LastErrorToString(error)
            ));
            return {};
        }

        return dumpPath;
    }

    LONG WINAPI TopLevelUnhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
        DebugExceptionHandler::WriteCrashReport(exceptionInfo, "Unhandled native exception");
        return EXCEPTION_EXECUTE_HANDLER;
    }

    void __cdecl TerminateHandler() {
        std::string reason = "std::terminate called";

        try {
            auto currentException = std::current_exception();
            if (currentException) {
                std::rethrow_exception(currentException);
            }
        }
        catch (std::exception const& e) {
            reason = std::format("std::terminate called after C++ exception: {}", e.what());
        }
        catch (...) {
            reason = "std::terminate called after unknown C++ exception";
        }

        CONTEXT context = {};
        if (g_bHasCxxExceptionContext) {
            context = g_CxxExceptionContext;
        }
        else {
            RtlCaptureContext(&context);
        }

        EXCEPTION_RECORD exceptionRecord = {};
        exceptionRecord.ExceptionCode = cppExceptionCode;
#if defined(_M_X64)
        exceptionRecord.ExceptionAddress = reinterpret_cast<void*>(context.Rip);
#elif defined(_M_IX86)
        exceptionRecord.ExceptionAddress = reinterpret_cast<void*>(context.Eip);
#endif

        EXCEPTION_POINTERS exceptionInfo = {};
        exceptionInfo.ExceptionRecord = &exceptionRecord;
        exceptionInfo.ContextRecord = &context;

        DebugExceptionHandler::WriteCrashReport(&exceptionInfo, reason);
        ExitProcess(1);
    }
}

void __cdecl translate_seh_to_cpp_exception(unsigned int, EXCEPTION_POINTERS* pExp) {
    throw StructuredException(pExp);
}

__declspec(thread) CONTEXT g_CxxExceptionContext = {};
__declspec(thread) bool g_bHasCxxExceptionContext = false;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    if (pExceptionInfo && pExceptionInfo->ExceptionRecord &&
        pExceptionInfo->ExceptionRecord->ExceptionCode == cppExceptionCode &&
        pExceptionInfo->ContextRecord) {
        g_CxxExceptionContext = *pExceptionInfo->ContextRecord;
        g_bHasCxxExceptionContext = true;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void DebugExceptionHandler::Install() {
    try {
        std::filesystem::create_directories(CrashPath());
    }
    catch (...) {
    }

    _set_se_translator(translate_seh_to_cpp_exception);

    if (!vectoredHandler) {
        vectoredHandler = AddVectoredExceptionHandler(1, VectoredExceptionHandler);
    }

    previousUnhandledExceptionFilter = SetUnhandledExceptionFilter(TopLevelUnhandledExceptionFilter);
    previousTerminateHandler = std::set_terminate(TerminateHandler);

    EnterSymbolLock();
    EnsureSymbolsLocked();
    LeaveSymbolLock();
}

void DebugExceptionHandler::Uninstall() {
    if (vectoredHandler) {
        RemoveVectoredExceptionHandler(vectoredHandler);
        vectoredHandler = nullptr;
    }

    SetUnhandledExceptionFilter(previousUnhandledExceptionFilter);
    if (previousTerminateHandler) {
        std::set_terminate(previousTerminateHandler);
        previousTerminateHandler = nullptr;
    }

    EnterSymbolLock();
    if (symbolsInitialized && ownsSymbolHandler) {
        SymCleanup(GetCurrentProcess());
    }
    symbolsInitialized = false;
    ownsSymbolHandler = false;
    LeaveSymbolLock();
}

bool DebugExceptionHandler::IsHandlingCrash() {
    return crashHandlerEntered.load() != 0;
}

std::string DebugExceptionHandler::GenerateStackTrace(CONTEXT* contextArg) {
    HANDLE process = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();

    CONTEXT context = {};
    if (contextArg) {
        context = *contextArg;
    }
    else {
        RtlCaptureContext(&context);
    }

    STACKFRAME64 stackFrame = {};
    DWORD machineType = 0;
    InitializeStackFrame(context, stackFrame, machineType);

    std::ostringstream stackTrace;
    stackTrace << "\n--- Stack Trace ---\n";

    EnterSymbolLock();
    bool haveSymbols = EnsureSymbolsLocked();

    for (int frameIndex = 0; frameIndex < 128; frameIndex++) {
        if (!StackWalk64(
            machineType,
            process,
            thread,
            &stackFrame,
            &context,
            nullptr,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            nullptr
        )) {
            break;
        }

        if (stackFrame.AddrPC.Offset == 0) {
            break;
        }

        stackTrace << "#" << frameIndex << " ";
        if (haveSymbols) {
            stackTrace << FormatStackFrame(process, stackFrame.AddrPC.Offset);
        }
        else {
            stackTrace << FormatAddress(stackFrame.AddrPC.Offset);
        }
        stackTrace << "\n";
    }

    LeaveSymbolLock();
    return stackTrace.str();
}

std::filesystem::path DebugExceptionHandler::WriteCrashReport(EXCEPTION_POINTERS* exceptionInfo, std::string_view reason) {
    if (crashHandlerEntered.exchange(1) != 0) {
        AppendRawLog(std::format(
            "[{}] [FATAL] Crash handler re-entered while handling another crash.\n",
            MakeTimestamp(false)
        ));
        return {};
    }

    CONTEXT context = {};
    if (exceptionInfo && exceptionInfo->ContextRecord) {
        context = *exceptionInfo->ContextRecord;
    }
    else {
        RtlCaptureContext(&context);
    }

    auto baseName = std::format(
        "LatiteCrash-{}-pid{}-tid{}",
        MakeTimestamp(true),
        GetCurrentProcessId(),
        GetCurrentThreadId()
    );

    auto dumpPath = WriteMiniDump(exceptionInfo, baseName);

    std::ostringstream report;
    report << "\n";
    report << "========== Latite Client Crash Report ==========\n";
    report << "Time: " << MakeTimestamp(false) << "\n";
    report << "Reason: " << reason << "\n";
    report << "Process ID: " << GetCurrentProcessId() << "\n";
    report << "Thread ID: " << GetCurrentThreadId() << "\n";

    if (exceptionInfo && exceptionInfo->ExceptionRecord) {
        auto exceptionRecord = exceptionInfo->ExceptionRecord;
        report << "Exception Code: 0x" << std::hex << std::uppercase << exceptionRecord->ExceptionCode << std::dec << "\n";
        report << "Exception Address: " << FormatAddress(reinterpret_cast<DWORD64>(exceptionRecord->ExceptionAddress)) << "\n";
    }

    auto latiteModulePath = GetModuleFilePath(GetLatiteModule());
    if (!latiteModulePath.empty()) {
        report << "Latite Module: " << latiteModulePath.string() << "\n";
    }

    if (!dumpPath.empty()) {
        report << "Minidump: " << dumpPath.string() << "\n";
    }
    else {
        report << "Minidump: unavailable\n";
    }

    report << GenerateStackTrace(&context);
    report << "==============================================\n";

    AppendRawLog(report.str());
    return dumpPath;
}

void LogExceptionDetails(StructuredException& ex) {
    DebugExceptionHandler::WriteCrashReport(ex.getExceptionPointers(), "Caught SEH exception at Latite error boundary");
}

void LogExceptionDetails(const std::exception& e) {
    CONTEXT context = {};
    if (g_bHasCxxExceptionContext) {
        context = g_CxxExceptionContext;
        g_bHasCxxExceptionContext = false;
    }
    else {
        RtlCaptureContext(&context);
    }

    EXCEPTION_RECORD exceptionRecord = {};
    exceptionRecord.ExceptionCode = cppExceptionCode;
#if defined(_M_X64)
    exceptionRecord.ExceptionAddress = reinterpret_cast<void*>(context.Rip);
#elif defined(_M_IX86)
    exceptionRecord.ExceptionAddress = reinterpret_cast<void*>(context.Eip);
#endif

    EXCEPTION_POINTERS exceptionInfo = {};
    exceptionInfo.ExceptionRecord = &exceptionRecord;
    exceptionInfo.ContextRecord = &context;

    DebugExceptionHandler::WriteCrashReport(
        &exceptionInfo,
        std::format("Caught C++ exception at Latite error boundary: {}", e.what())
    );
}

#endif
