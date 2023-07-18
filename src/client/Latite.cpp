// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"
#include "util/Logger.h"

#include "feature/module/ModuleManager.h"
#include "feature/command/CommandManager.h"
#include "misc/ClientMessageSink.h"
#include "hook/Hooks.h"
#include "event/Eventing.h"

#include "sdk/common/client/game/ClientInstance.h"

using namespace std;

namespace {
    alignas(Latite) char latiteBuf[sizeof(Latite)] = {};
    alignas(ModuleManager) char mmgrBuf[sizeof(ModuleManager)] = {};
    alignas(ClientMessageSink) char messageSinkBuf[sizeof(ClientMessageSink)] = {};
    alignas(CommandManager) char commandMgrBuf[sizeof(CommandManager)] = {};
    alignas(SettingGroup) char mainSettingGroup[sizeof(SettingGroup)] = {};
    alignas(LatiteHooks) char hooks[sizeof(LatiteHooks)] = {};
    alignas(Eventing) char eventing[sizeof(Eventing)] = {};
}

DWORD __stdcall startThread(HINSTANCE dll) {
    new (latiteBuf) Latite;
    new (mmgrBuf) ModuleManager;
    new (messageSinkBuf) ClientMessageSink;
    new (commandMgrBuf) CommandManager;
    new (mainSettingGroup) SettingGroup("global");
    new (hooks) LatiteHooks();

    Logger::setup();

    Logger::info("Waiting for game to load..");

    while (!sdk::ClientInstance::get()) {
    }

    Latite::get().initialize(dll);

    Logger::info("Initialized Latite Client");

    return 0ul;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID)  // reserved
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startThread, hinstDLL, 0, nullptr));
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        // Remove singletons
        Latite::get().getHooks().disable();

        Latite::getModuleManager().~ModuleManager();
        Latite::getClientMessageSink().~ClientMessageSink();
        Latite::getCommandManager().~CommandManager();
        Latite::getSettings().~SettingGroup();
        Latite::getHooks().~LatiteHooks();
        Latite::getEventing().~Eventing();
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
}

SettingGroup& Latite::getSettings() noexcept
{
    return *std::launder(reinterpret_cast<SettingGroup*>(mainSettingGroup));
}

LatiteHooks& Latite::getHooks() noexcept
{
    return *std::launder(reinterpret_cast<LatiteHooks*>(hooks));
}

Eventing& Latite::getEventing() noexcept
{
    return *std::launder(reinterpret_cast<Eventing*>(eventing));
}

void Latite::doEject() noexcept
{
    Latite::get().getHooks().uninit();

    FreeLibrary(this->dllInst);
}

void Latite::queueEject() noexcept
{
    this->shouldEject = true;
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, dllInst, 0, nullptr);
}

void Latite::initialize(HINSTANCE hInst)
{
    this->dllInst = hInst;
    getHooks().init();
    Logger::info("Initialized Hooks");
    getHooks().enable();
    Logger::info("Enabled Hooks");
}
