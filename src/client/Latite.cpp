// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"
#include "util/util.h"
#include "util/Logger.h"

#include "feature/module/ModuleManager.h"
#include "feature/command/CommandManager.h"
#include "config/ConfigManager.h"
#include "misc/ClientMessageSink.h"
#include "hook/Hooks.h"
#include "event/Eventing.h"
#include "event/impl/RenderGameEvent.h"

#include "sdk/signature/storage.h"

#include "sdk/common/client/game/ClientInstance.h"
#include <winrt/windows.ui.viewmanagement.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/windows.ui.popups.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

#include "misc/AuthWindow.h"
#include "render/Renderer.h"

using namespace std;

namespace {
    alignas(Latite) char latiteBuf[sizeof(Latite)] = {};
    alignas(ModuleManager) char mmgrBuf[sizeof(ModuleManager)] = {};
    alignas(ClientMessageSink) char messageSinkBuf[sizeof(ClientMessageSink)] = {};
    alignas(CommandManager) char commandMgrBuf[sizeof(CommandManager)] = {};
    alignas(ConfigManager) char configMgrBuf[sizeof(ConfigManager)] = {};
    alignas(SettingGroup) char mainSettingGroup[sizeof(SettingGroup)] = {};
    alignas(LatiteHooks) char hooks[sizeof(LatiteHooks)] = {};
    alignas(Eventing) char eventing[sizeof(Eventing)] = {};
    alignas(Renderer) char rendererBuf[sizeof(Renderer)] = {};
}

DWORD __stdcall startThread(HINSTANCE dll) {
    // Needed for Logger
    new (messageSinkBuf) ClientMessageSink;
    new (eventing) Eventing();
    new (latiteBuf) Latite;

    Logger::setup();

    winrt::Windows::ApplicationModel::Package package = winrt::Windows::ApplicationModel::Package::Current();
    winrt::Windows::ApplicationModel::PackageVersion version = package.Id().Version();
    
    std::string gameVersion = "";
    {
        std::string rev = std::to_string(version.Build);
        std::string rem = rev.substr(0, rev.size() - 2); // remove 2 digits from end

        int ps = std::stoi(rem);
        std::stringstream ss;
        ss << version.Major << "." << version.Minor << "." << ps;// hacky
        gameVersion = ss.str();
    }
#if LATITE_DEBUG
    Logger::info("Resolving signatures..");
#endif


    int sigCount = 0;
    int deadCount = 0;

    // TODO: game version -> array
    std::unordered_map<std::string, std::vector<SigImpl*>> versMap = { { "1.20.12", {&Signatures::Misc::clientInstance, &Signatures::Keyboard_feed, &Signatures::LevelRenderer_renderLevel,
        &Signatures::Offset::LevelRendererPlayer_fovX, &Signatures::Offset::LevelRendererPlayer_origin, &Signatures::Offset::MinecraftGame_cursorGrabbed,
        &Signatures::Options_getGamma }}, { "1.18.12", {} } };


    std::vector<SigImpl*> sigList = versMap.begin()->second;

    if (versMap.contains(gameVersion)) {
        sigList = versMap[gameVersion];
    }
    else {
        std::stringstream ss;
        ss << "Latite Client does not support your version: " << gameVersion << ". Latite only supports the following versions:\n\n";

        for (auto& key : versMap) {
            ss << key.first << "\n";
        }
        ss << "\nContinue at your own risk of crashing.";

        MessageBoxA(NULL, ss.str().c_str(), "Warning", MB_ICONINFORMATION | MB_OK);
    }


    new (mmgrBuf) ModuleManager;
    new (commandMgrBuf) CommandManager;
    new (mainSettingGroup) SettingGroup("global");
    new (configMgrBuf) ConfigManager();
    new (hooks) LatiteHooks();
    new (rendererBuf) Renderer();

    AuthWindow wnd{ Latite::get().dllInst };

    wnd.show();
    wnd.runMessagePump();

    for (auto& entry : sigList) {
        if (!entry->resolve()) {
            Logger::warn("Signature {} failed to resolve! Pattern: {}", entry->name, entry->signature);
            deadCount++;
        }
        else {
            sigCount++;
        }
    }
#if LATITE_DEBUG
    Logger::info("Resolved {} signatures ({} dead)", sigCount, deadCount);
#endif

    Logger::info("Waiting for game to load..");

    while (!sdk::ClientInstance::get()) {
    }

    Latite::get().initialize(dll);

    if (!Latite::getConfigManager().loadMaster()) {
        Logger::fatal("Could not load master config!");
    }
    else {
        Logger::info("Loaded master config");
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
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startThread, hinstDLL, 0, nullptr));
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        // Remove singletons
        Latite::getHooks().disable();

        // Wait for hooks to disable
        std::this_thread::sleep_for(200ms);

        Latite::getConfigManager().saveCurrentConfig();

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

ConfigManager& Latite::getConfigManager() noexcept
{
    return *std::launder(reinterpret_cast<ConfigManager*>(configMgrBuf));
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

Renderer& Latite::getRenderer() noexcept
{
    return *std::launder(reinterpret_cast<Renderer*>(rendererBuf));
}

void Latite::doEject() noexcept
{
    Latite::get().getHooks().uninit();

    FreeLibrary(this->dllInst);
}

void Latite::queueEject() noexcept
{
    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    app.Title(L"");
    this->shouldEject = true;
    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, dllInst, 0, nullptr);
}

void Latite::initialize(HINSTANCE hInst) {
    this->dllInst = hInst;
    getHooks().init();
    Logger::info("Initialized Hooks");
    getHooks().enable();
    Logger::info("Enabled Hooks");

    // TODO: use UpdateEvent
    Latite::getEventing().listen<RenderGameEvent>(this, (EventListenerFunc) & Latite::onUpdate);
}

void Latite::threadsafeInit() {
    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    std::string vstr(this->version);
    auto ws = util::strToWstr("Latite Client " + vstr);
    app.Title(ws);
}

void Latite::onUpdate(Event&) {
    if (!hasInit) {
        threadsafeInit();
        hasInit = true;
    }
}

void Latite::loadConfig(SettingGroup&) {
}
