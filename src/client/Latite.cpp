// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"
#include "util/Logger.h"

#include "module/ModuleManager.h"

using namespace std;

alignas(Latite) char latiteBuf[sizeof(Latite)] = {};
alignas(ModuleManager) char mmgrBuf[sizeof(ModuleManager)] = {};

DWORD __stdcall startThread(HINSTANCE dll) {
    new (latiteBuf) Latite;
    new (mmgrBuf) ModuleManager;
    Latite::get().initialize(dll);
    Logger::setup();
    Logger::info("Initializing Latite Client {}");

    return 0ul;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startThread), hinstDLL, 0, nullptr));
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        // Remove singletons
        Latite::get().getModuleManager().~ModuleManager();
        Latite::get().~Latite();
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

Latite& Latite::get() noexcept
{
    return *std::launder(reinterpret_cast<Latite*>(latiteBuf));
}

ModuleManager& Latite::getModuleManager() noexcept
{
    return *std::launder(reinterpret_cast<ModuleManager*>(mmgrBuf));
}

void Latite::doEject() noexcept
{
    FreeLibrary(this->dllInst);
    onUnload();
}

void Latite::initialize(HINSTANCE hInst)
{
    this->dllInst = hInst;
}

void Latite::onUnload()
{
    // Save config and everything
}
