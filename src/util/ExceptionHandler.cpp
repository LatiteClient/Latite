#include "pch.h"
#include "ExceptionHandler.h"

#ifdef LATITE_CRASH_REPORTING
#include "util/Util.h"

#include <atomic>
#include <cstdlib>
#include <dbghelp.h>
#include <iomanip>
#include <iterator>
#include <psapi.h>

namespace {
    // MSVC's SEH exception code for a C++ exception
    constexpr DWORD cppExceptionCode = 0xE06D7363;

    PVOID vectoredHandler = nullptr;
    LPTOP_LEVEL_EXCEPTION_FILTER previousUnhandledExceptionFilter = nullptr;
    std::terminate_handler previousTerminateHandler = nullptr;
    bool handlersInstalled = false;

    INIT_ONCE handlerLockInit = INIT_ONCE_STATIC_INIT;
    CRITICAL_SECTION handlerLock = {};
    INIT_ONCE symbolLockInit = INIT_ONCE_STATIC_INIT;
    CRITICAL_SECTION symbolLock = {};
    bool symbolsInitialized = false;
    bool ownsSymbolHandler = false;
    std::atomic_long crashHandlerEntered = 0;

    BOOL CALLBACK InitHandlerLock(PINIT_ONCE, PVOID, PVOID*) {
        InitializeCriticalSection(&handlerLock);
        return TRUE;
    }

    BOOL CALLBACK InitSymbolLock(PINIT_ONCE, PVOID, PVOID*) {
        InitializeCriticalSection(&symbolLock);
        return TRUE;
    }

    void EnterHandlerLock() {
        InitOnceExecuteOnce(&handlerLockInit, InitHandlerLock, nullptr, nullptr);
        EnterCriticalSection(&handlerLock);
    }

    void LeaveHandlerLock() {
        LeaveCriticalSection(&handlerLock);
    }

    void EnterSymbolLock() {
        InitOnceExecuteOnce(&symbolLockInit, InitSymbolLock, nullptr, nullptr);
        EnterCriticalSection(&symbolLock);
    }

    void LeaveSymbolLock() {
        LeaveCriticalSection(&symbolLock);
    }

    class CriticalSectionGuard {
    public:
        explicit CriticalSectionGuard(void (*enterFunc)(), void (*leaveFunc)())
            : m_leaveFunc(leaveFunc) {
            enterFunc();
        }

        CriticalSectionGuard(CriticalSectionGuard const&) = delete;
        CriticalSectionGuard& operator=(CriticalSectionGuard const&) = delete;

        ~CriticalSectionGuard() {
            m_leaveFunc();
        }

    private:
        void (*m_leaveFunc)();
    };

    std::string LastErrorToString(DWORD error) {
        return std::format("{}", error);
    }

    std::string WideToUtf8(std::wstring const& value) {
        if (value.empty()) {
            return {};
        }

        int size = WideCharToMultiByte(
            CP_UTF8,
            0,
            value.c_str(),
            static_cast<int>(value.size()),
            nullptr,
            0,
            nullptr,
            nullptr
        );
        if (size <= 0) {
            return {};
        }

        std::string result(static_cast<size_t>(size), '\0');
        WideCharToMultiByte(
            CP_UTF8,
            0,
            value.c_str(),
            static_cast<int>(value.size()),
            result.data(),
            size,
            nullptr,
            nullptr
        );
        return result;
    }

    std::string WideToUtf8(wchar_t const* value) {
        if (!value || !*value) {
            return {};
        }

        return WideToUtf8(std::wstring(value));
    }

    std::string PathToUtf8(std::filesystem::path const& path) {
        return WideToUtf8(path.wstring());
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

    constexpr std::string_view crashReportTitle = "Latite Client Crash Report";
    constexpr size_t crashReportWidth = 75;

    std::string MakeCenteredDivider(std::string_view title, char fill, size_t width) {
        if (title.empty()) {
            return std::string(width, fill);
        }

        std::string decoratedTitle = std::format(" {} ", title);
        if (decoratedTitle.size() >= width) {
            return decoratedTitle;
        }

        size_t totalFill = width - decoratedTitle.size();
        size_t leftFill = totalFill / 2;
        size_t rightFill = totalFill - leftFill;
        return std::string(leftFill, fill) + decoratedTitle + std::string(rightFill, fill);
    }

    void AppendSectionHeading(std::ostringstream& stream, std::string_view title) {
        stream << title << "\n";
        stream << std::string(title.size(), '-') << "\n";
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
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetLatiteModule),
            &module
        );

        return module;
    }

    std::filesystem::path GetModuleFilePath(HMODULE module) {
        wchar_t path[MAX_PATH] = {};
        if (!module || !GetModuleFileNameW(module, path, static_cast<DWORD>(std::size(path)))) {
            return {};
        }

        return path;
    }

    void AddSymbolPathPart(std::wstring& symbolPath, std::filesystem::path const& path) {
        if (path.empty()) {
            return;
        }

        auto part = path.wstring();
        if (part.empty()) {
            return;
        }

        if (!symbolPath.empty()) {
            symbolPath += L';';
        }
        symbolPath += part;
    }

    void AddSymbolPathPart(std::wstring& symbolPath, wchar_t const* path) {
        if (!path || !*path) {
            return;
        }

        if (!symbolPath.empty()) {
            symbolPath += L';';
        }
        symbolPath += path;
    }

    std::wstring BuildSymbolPath() {
        std::wstring symbolPath;

        auto latiteModulePath = GetModuleFilePath(GetLatiteModule());
        AddSymbolPathPart(symbolPath, latiteModulePath.parent_path());
        AddSymbolPathPart(symbolPath, util::GetLatitePath());
        AddSymbolPathPart(symbolPath, CrashPath());

        wchar_t currentDirectory[MAX_PATH] = {};
        if (GetCurrentDirectoryW(static_cast<DWORD>(std::size(currentDirectory)), currentDirectory)) {
            AddSymbolPathPart(symbolPath, currentDirectory);
        }

        wchar_t ntSymbolPath[32767] = {};
        if (GetEnvironmentVariableW(L"_NT_SYMBOL_PATH", ntSymbolPath, static_cast<DWORD>(std::size(ntSymbolPath)))) {
            AddSymbolPathPart(symbolPath, ntSymbolPath);
        }

        wchar_t ntAltSymbolPath[32767] = {};
        if (GetEnvironmentVariableW(L"_NT_ALT_SYMBOL_PATH", ntAltSymbolPath, static_cast<DWORD>(std::size(ntAltSymbolPath)))) {
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

        SymLoadModuleExW(
            process,
            nullptr,
            modulePath.wstring().c_str(),
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

        if (SymInitializeW(process, symbolPath.empty() ? nullptr : symbolPath.c_str(), TRUE)) {
            symbolsInitialized = true;
            ownsSymbolHandler = true;
            EnsureLatiteModuleLoaded(process);
            return true;
        }

        DWORD error = GetLastError();
        if (error == ERROR_INVALID_PARAMETER) {
            // DbgHelp is process-global. If something else initialized it first,
            // keep using that session and update the search path for LatiteDebug.pdb.
            SymSetSearchPathW(process, symbolPath.c_str());
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
        IMAGEHLP_MODULEW64 moduleInfo = {};
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);

        if (SymGetModuleInfoW64(process, address, &moduleInfo)) {
            std::string moduleName;
            if (*moduleInfo.ImageName) {
                moduleName = PathToUtf8(std::filesystem::path(moduleInfo.ImageName).filename());
            }
            else {
                moduleName = WideToUtf8(moduleInfo.ModuleName);
            }

            auto offset = address - moduleInfo.BaseOfImage;
            std::ostringstream oss;
            oss << moduleName << "+0x" << std::hex << std::uppercase << offset;
            return oss.str();
        }

        return FormatAddress(address);
    }

    std::string FormatStackFrame(HANDLE process, DWORD64 address) {
        alignas(SYMBOL_INFOW) char symbolBuffer[sizeof(SYMBOL_INFOW) + (MAX_SYM_NAME * sizeof(wchar_t))] = {};
        auto symbol = reinterpret_cast<PSYMBOL_INFOW>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(SYMBOL_INFOW);
        symbol->MaxNameLen = MAX_SYM_NAME;

        std::ostringstream frame;
        DWORD64 displacement = 0;

        if (SymFromAddrW(process, address, &displacement, symbol)) {
            frame << FormatModuleOffset(process, address) << " ";
            frame << WideToUtf8(std::wstring(symbol->Name, symbol->NameLen));

            if (displacement != 0) {
                frame << "+0x" << std::hex << std::uppercase << displacement << std::dec;
            }
        }
        else {
            frame << FormatModuleOffset(process, address);
        }

        IMAGEHLP_LINEW64 line = {};
        line.SizeOfStruct = sizeof(line);
        DWORD lineDisplacement = 0;
        if (SymGetLineFromAddrW64(process, address, &lineDisplacement, &line)) {
            frame << " in " << WideToUtf8(line.FileName) << ":" << line.LineNumber;
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
        auto tempDumpPath = CrashPath() / (baseName + ".dmp.tmp");
        HANDLE dumpFile = CreateFileW(
            tempDumpPath.wstring().c_str(),
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
                PathToUtf8(tempDumpPath),
                LastErrorToString(GetLastError())
            ));
            return {};
        }

        MINIDUMP_EXCEPTION_INFORMATION dumpException = {};
        dumpException.ThreadId = GetCurrentThreadId();
        dumpException.ExceptionPointers = exceptionInfo;
        dumpException.ClientPointers = FALSE;

        // Keep the primary dump conservative. More aggressive flags such as
        // MiniDumpWithIndirectlyReferencedMemory can fault again when the crash
        // was caused by corrupt SDK pointers or invalid offsets.
        auto dumpType = static_cast<MINIDUMP_TYPE>(
            MiniDumpNormal |
            MiniDumpWithDataSegs |
            MiniDumpWithThreadInfo |
            MiniDumpWithUnloadedModules |
            MiniDumpWithFullMemoryInfo
        );

        BOOL wroteDump = FALSE;
        {
            CriticalSectionGuard symbolGuard(EnterSymbolLock, LeaveSymbolLock);
            wroteDump = MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                dumpFile,
                dumpType,
                exceptionInfo ? &dumpException : nullptr,
                nullptr,
                nullptr
            );
        }

        DWORD error = wroteDump ? ERROR_SUCCESS : GetLastError();
        FlushFileBuffers(dumpFile);
        CloseHandle(dumpFile);

        if (!wroteDump) {
            AppendRawLog(std::format(
                "[{}] [FATAL] MiniDumpWriteDump failed for {}. Error: {}\n",
                MakeTimestamp(false),
                PathToUtf8(tempDumpPath),
                LastErrorToString(error)
            ));
            DeleteFileW(tempDumpPath.wstring().c_str());
            return {};
        }

        if (!MoveFileExW(tempDumpPath.wstring().c_str(), dumpPath.wstring().c_str(), MOVEFILE_REPLACE_EXISTING)) {
            AppendRawLog(std::format(
                "[{}] [FATAL] Failed to finalize minidump {}. Error: {}\n",
                MakeTimestamp(false),
                PathToUtf8(dumpPath),
                LastErrorToString(GetLastError())
            ));
            DeleteFileW(tempDumpPath.wstring().c_str());
            return {};
        }

        return dumpPath;
    }

    LONG WINAPI TopLevelUnhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
        DebugExceptionHandler::WriteCrashReport(exceptionInfo, "Unhandled native exception");

        auto previousFilter = previousUnhandledExceptionFilter;
        if (previousFilter && previousFilter != TopLevelUnhandledExceptionFilter) {
            return previousFilter(exceptionInfo);
        }

        return EXCEPTION_CONTINUE_SEARCH;
    }

    void __cdecl TerminateHandler() {
        std::string reason = "std::terminate called";
        bool hasCurrentException = false;

        try {
            auto currentException = std::current_exception();
            if (currentException) {
                hasCurrentException = true;
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
        if (hasCurrentException && g_bHasCxxExceptionContext) {
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

        auto previousHandler = previousTerminateHandler;
        if (previousHandler && previousHandler != TerminateHandler) {
            previousHandler();
        }

        DebugExceptionHandler::AbortProcess();
    }

    int HandleSehException(EXCEPTION_POINTERS* exceptionInfo, char const* reason) noexcept {
        if (exceptionInfo && exceptionInfo->ExceptionRecord &&
            exceptionInfo->ExceptionRecord->ExceptionCode == cppExceptionCode) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        try {
            DebugExceptionHandler::WriteCrashReport(
                exceptionInfo,
                reason ? reason : "Caught SEH exception at Latite native boundary"
            );
        }
        catch (...) {
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }
}

__declspec(thread) CONTEXT g_CxxExceptionContext = {};
__declspec(thread) bool g_bHasCxxExceptionContext = false;
__declspec(thread) int g_ErrorBoundaryDepth = 0;

LONG WINAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    if (pExceptionInfo && pExceptionInfo->ExceptionRecord &&
        pExceptionInfo->ExceptionRecord->ExceptionCode == cppExceptionCode &&
        pExceptionInfo->ContextRecord) {
        g_CxxExceptionContext = *pExceptionInfo->ContextRecord;
        g_bHasCxxExceptionContext = true;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void DebugExceptionHandler::PrepareErrorBoundary() {
    g_CxxExceptionContext = {};
    g_bHasCxxExceptionContext = false;
}

DebugExceptionHandler::ErrorBoundaryScope::ErrorBoundaryScope() {
    PrepareErrorBoundary();
    ++g_ErrorBoundaryDepth;
}

DebugExceptionHandler::ErrorBoundaryScope::~ErrorBoundaryScope() {
    if (g_ErrorBoundaryDepth > 0) {
        --g_ErrorBoundaryDepth;
    }

    if (g_ErrorBoundaryDepth == 0) {
        g_CxxExceptionContext = {};
        g_bHasCxxExceptionContext = false;
    }
}

void DebugExceptionHandler::Install() {
    try {
        std::filesystem::create_directories(CrashPath());
    }
    catch (...) {
    }

    {
        CriticalSectionGuard handlerGuard(EnterHandlerLock, LeaveHandlerLock);
        if (handlersInstalled) {
            return;
        }

        vectoredHandler = AddVectoredExceptionHandler(1, VectoredExceptionHandler);
        previousUnhandledExceptionFilter = SetUnhandledExceptionFilter(TopLevelUnhandledExceptionFilter);
        previousTerminateHandler = std::set_terminate(TerminateHandler);
        handlersInstalled = true;
    }
}

void DebugExceptionHandler::Uninstall() {
    {
        CriticalSectionGuard handlerGuard(EnterHandlerLock, LeaveHandlerLock);
        if (!handlersInstalled) {
            return;
        }

        if (vectoredHandler) {
            RemoveVectoredExceptionHandler(vectoredHandler);
            vectoredHandler = nullptr;
        }

        auto currentUnhandledFilter = SetUnhandledExceptionFilter(previousUnhandledExceptionFilter);
        if (currentUnhandledFilter != TopLevelUnhandledExceptionFilter) {
            SetUnhandledExceptionFilter(currentUnhandledFilter);
        }

        if (previousTerminateHandler) {
            auto currentTerminateHandler = std::set_terminate(previousTerminateHandler);
            if (currentTerminateHandler != TerminateHandler) {
                std::set_terminate(currentTerminateHandler);
            }
        }

        previousUnhandledExceptionFilter = nullptr;
        previousTerminateHandler = nullptr;
        handlersInstalled = false;
    }

    CriticalSectionGuard symbolGuard(EnterSymbolLock, LeaveSymbolLock);
    // DbgHelp is process-global. Leave any initialized session alive rather
    // than tearing it down under another component in the host process.
}

bool DebugExceptionHandler::IsHandlingCrash() {
    return crashHandlerEntered.load() != 0;
}

[[noreturn]] void DebugExceptionHandler::AbortProcess() {
    std::abort();
    TerminateProcess(GetCurrentProcess(), 1);
    for (;;) {
        Sleep(INFINITE);
    }
}

std::uintptr_t DebugExceptionHandler::RunWithSehGuard(SehCallback callback, void* context, char const* reason) noexcept {
    __try {
        if (callback) {
            return callback(context);
        }
    }
    __except (HandleSehException(GetExceptionInformation(), reason)) {
        AbortProcess();
    }

    return 0;
}

void DebugExceptionHandler::RunVoidWithSehGuard(SehVoidCallback callback, void* context, char const* reason) noexcept {
    __try {
        if (callback) {
            callback(context);
        }
    }
    __except (HandleSehException(GetExceptionInformation(), reason)) {
        AbortProcess();
    }
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
    AppendSectionHeading(stackTrace, "Stack Trace");

    CriticalSectionGuard symbolGuard(EnterSymbolLock, LeaveSymbolLock);
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

    return stackTrace.str();
}

std::filesystem::path DebugExceptionHandler::WriteCrashReport(EXCEPTION_POINTERS* exceptionInfo, std::string_view reason) {
    if (crashHandlerEntered.exchange(1) != 0) {
        std::ostringstream reentryReport;
        reentryReport << "[" << MakeTimestamp(false) << "] [FATAL] Crash handler re-entered while handling another crash.";
        if (exceptionInfo && exceptionInfo->ExceptionRecord) {
            reentryReport << " Re-entry code: 0x" << std::hex << std::uppercase
                << exceptionInfo->ExceptionRecord->ExceptionCode;
            reentryReport << " at " << FormatAddress(reinterpret_cast<DWORD64>(
                exceptionInfo->ExceptionRecord->ExceptionAddress));
        }
        reentryReport << "\n";
        AppendRawLog(reentryReport.str());
        return {};
    }

    CONTEXT context = {};
    if (exceptionInfo && exceptionInfo->ContextRecord) {
        context = *exceptionInfo->ContextRecord;
    }
    else {
        RtlCaptureContext(&context);
    }

#if defined(LATITE_NIGHTLY)
    std::string baseName = std::format(
        "LatiteNightlyCrash-{}",
        MakeTimestamp(true),
        GetCurrentProcessId(),
        GetCurrentThreadId()
    );
#elif defined(LATITE_DEBUG)
    std::string baseName = std::format(
        "LatiteDebugCrash-{}",
        MakeTimestamp(true),
        GetCurrentProcessId(),
        GetCurrentThreadId()
    );
#else
    std::string baseName = std::format(
        "LatiteCrash-{}",
        MakeTimestamp(true),
        GetCurrentProcessId(),
        GetCurrentThreadId()
    );
#endif

    auto attemptedDumpPath = CrashPath() / (baseName + ".dmp");
    auto dumpPath = WriteMiniDump(exceptionInfo, baseName);

    std::ostringstream report;
    report << "\n";
    report << MakeCenteredDivider(crashReportTitle, '=', crashReportWidth) << "\n\n";
    AppendSectionHeading(report, "Crash Summary");
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
        report << "Latite Module: " << PathToUtf8(latiteModulePath) << "\n";
    }

    report << "Minidump: attempting " << PathToUtf8(attemptedDumpPath) << "\n";
    if (!dumpPath.empty()) {
        report << "Minidump Result: written " << PathToUtf8(dumpPath) << "\n";
    }
    else {
        report << "Minidump Result: unavailable\n";
    }
    report << "\n";
    AppendRawLog(report.str());

    std::ostringstream stackReport;
    try {
        stackReport << GenerateStackTrace(&context);
    }
    catch (std::exception const& e) {
        AppendSectionHeading(stackReport, "Stack Trace");
        stackReport << "Stack trace unavailable: " << e.what() << "\n";
    }
    catch (...) {
        AppendSectionHeading(stackReport, "Stack Trace");
        stackReport << "Stack trace unavailable due to an unknown error.\n";
    }
    stackReport << "\n" << std::string(crashReportWidth, '=') << "\n";
    AppendRawLog(stackReport.str());

    return dumpPath;
}

void LogUnknownExceptionDetails(std::string_view reason) {
    CONTEXT context = {};
    RtlCaptureContext(&context);

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

    g_CxxExceptionContext = {};
    g_bHasCxxExceptionContext = false;

    DebugExceptionHandler::WriteCrashReport(&exceptionInfo, reason);
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

    g_CxxExceptionContext = {};
    g_bHasCxxExceptionContext = false;

    DebugExceptionHandler::WriteCrashReport(
        &exceptionInfo,
        std::format("Caught C++ exception at Latite error boundary: {}", e.what())
    );
}

#endif
