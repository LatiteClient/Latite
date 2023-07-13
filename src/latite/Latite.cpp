// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"

using namespace std;

alignas(Latite) char latiteBuf[sizeof(Latite)] = {};

DWORD __stdcall startThread(HINSTANCE dll) {
    new (latiteBuf) Latite;
    Latite::get().initialize(dll);
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CloseHandle(CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startThread), hinstDLL, 0, 0));
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

Latite& Latite::get()
{
    return *std::launder(reinterpret_cast<Latite*>(latiteBuf));
}

void Latite::doEject() noexcept
{
    FreeLibrary(this->dllInst);
}

void Latite::initialize(HINSTANCE hInst)
{
    this->dllInst = hInst;
}
