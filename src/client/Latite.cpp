// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"
#include "util/Logger.h"

#include "module/ModuleManager.h"
#include "command/CommandManager.h"
#include "misc/ClientMessageSink.h"

#include "sdk/common/client/game/ClientInstance.h"

using namespace std;

alignas(Latite) char latiteBuf[sizeof(Latite)] = {};
alignas(ModuleManager) char mmgrBuf[sizeof(ModuleManager)] = {};
alignas(ClientMessageSink) char messageSinkBuf[sizeof(ClientMessageSink)] = {};
alignas(CommandManager) char commandMgrBuf[sizeof(CommandManager)] = {};

DWORD __stdcall startThread(HINSTANCE dll) {
    new (latiteBuf) Latite;
    new (mmgrBuf) ModuleManager;
    new (messageSinkBuf) ClientMessageSink;
    new (commandMgrBuf) CommandManager;

    Latite::get().initialize(dll);
    Logger::setup();

    Logger::info("Waiting for game to load..");

    while (!sdk::ClientInstance::get()) {
    }

    Logger::info("Initialized Latite Client");

    return 0ul;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID)  // reserved
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startThread), hinstDLL, 0, nullptr));
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        // Remove singletons
        Latite::getModuleManager().~ModuleManager();
        Latite::getClientMessageSink().~ClientMessageSink();
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

CommandManager& Latite::getCommandManager() noexcept
{
    return *std::launder(reinterpret_cast<CommandManager*>(commandMgrBuf));
}

ClientMessageSink& Latite::getClientMessageSink() noexcept
{
    return *std::launder(reinterpret_cast<ClientMessageSink*>(messageSinkBuf));
    // TODO: insert return statement here
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
