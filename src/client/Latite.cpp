// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"
#include "util/util.h"
#include "util/Logger.h"

#include "feature/module/ModuleManager.h"
#include "feature/command/CommandManager.h"
#include "script/ScriptManager.h"

#include "config/ConfigManager.h"
#include "misc/ClientMessageSink.h"
#include "hook/Hooks.h"
#include "event/Eventing.h"
#include "event/impl/RenderGameEvent.h"
#include "event/impl/KeyUpdateEvent.h"
#include "event/impl/RendererInitEvent.h"
#include "event/impl/FocusLostEvent.h"
#include "event/impl/AppSuspendedEvent.h"

#include "sdk/signature/storage.h"

#include "sdk/common/client/game/ClientInstance.h"
#include <winrt/windows.ui.viewmanagement.h>
#include <winrt/Windows.ApplicationModel.Core.h>

#include <winrt/windows.ui.core.h>
#include <winrt/windows.ui.popups.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

#include "misc/AuthWindow.h"
#include "render/Renderer.h"
#include "screen/ScreenManager.h"
#include "render/Assets.h"

int sdk::internalVers = sdk::VLATEST;

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
    alignas(ScreenManager) char scnMgrBuf[sizeof(ScreenManager)] = {};
    alignas(Assets) char assetsBuf[sizeof(Assets)] = {};
    alignas(ScriptManager) char scriptMgrBuf[sizeof(ScriptManager)] = {};
}

#define MVSIG(...) ([]() -> std::pair<SigImpl*, SigImpl*> {\
if (sdk::internalVers == sdk::VLATEST) return {&Signatures::__VA_ARGS__, &Signatures::__VA_ARGS__};\
else { return {&Signatures_1_18_12::__VA_ARGS__, &Signatures::__VA_ARGS__}; }\
})()

DWORD __stdcall startThread(HINSTANCE dll) {
    // Needed for Logger
    new (messageSinkBuf) ClientMessageSink;
    new (eventing) Eventing();
    new (latiteBuf) Latite;

    std::filesystem::create_directory(util::GetLatitePath());
    std::filesystem::create_directory(util::GetLatitePath() / "Assets");
    Logger::Setup();

    Logger::Info("Loading assets");
    // ... init assets

    //


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
    Logger::Info("Resolving signatures..");
#endif


    int sigCount = 0;
    int deadCount = 0;

    std::unordered_map<std::string, sdk::Version> versNumMap = {
        { "1.20.12", sdk::VLATEST },
        { "1.20.10", sdk::VLATEST },
        { "1.18.12", sdk::V1_18_12 },
        { "1.18.10", sdk::V1_18_12 }
    };

    if (versNumMap.contains(gameVersion)) {
        auto vers =  versNumMap[gameVersion];
        sdk::internalVers = vers;
    }
    else {
        std::stringstream ss;
        ss << "Latite Client does not support your version: " << gameVersion << ". Latite only supports the following versions:\n\n";

        for (auto& key : versNumMap) {
            ss << key.first << "\n";
        }

        Logger::Warn(ss.str());
    }

    std::vector<std::pair<SigImpl*, SigImpl*>> sigList = {
        MVSIG(Misc::clientInstance),
        MVSIG(Keyboard_feed),
        MVSIG(LevelRenderer_renderLevel),
        MVSIG(Offset::LevelRendererPlayer_fovX),
        MVSIG(Offset::LevelRendererPlayer_origin),
        MVSIG(Offset::MinecraftGame_cursorGrabbed),
        MVSIG(Options_getGamma),
        MVSIG(ClientInstance_grabCursor),
        MVSIG(ClientInstance_releaseCursor),
        MVSIG(Level_tick),
        MVSIG(ChatScreenController_sendChatMessage),
        MVSIG(GameRenderer__renderCurrentFrame),
        MVSIG(onClick),
        MVSIG(AppPlatform__fireAppFocusLost),
        MVSIG(MinecraftGame_onAppSuspended),
            };
    
    new (mmgrBuf) ModuleManager;
    new (commandMgrBuf) CommandManager;
    new (mainSettingGroup) SettingGroup("global");
    new (configMgrBuf) ConfigManager();
    new (hooks) LatiteHooks();
    new (scnMgrBuf) ScreenManager(); // needs to be before renderer
    new (scriptMgrBuf) ScriptManager();
    new (rendererBuf) Renderer();
    new (assetsBuf) Assets();

    //AuthWindow wnd{ Latite::get().dllInst };

   // wnd.show();
    //wnd.runMessagePump();

    for (auto& entry : sigList) {
        if (!entry.first->mod) continue;
        auto res = entry.first->resolve();
        if (!res) {
            Logger::Warn("Signature {} failed to resolve! Pattern: {}", entry.first->name, entry.first->signature);
            deadCount++;
        }
        else {
            entry.second->result = entry.first->result;
            entry.second->scan_result = entry.first->scan_result;
            sigCount++;
        }
    }
#if LATITE_DEBUG
    Logger::Info("Resolved {} signatures ({} dead)", sigCount, deadCount);
#endif

    Logger::Info("Waiting for game to load..");

    while (!sdk::ClientInstance::get()) {
    }

    Latite::get().initialize(dll);

    if (!Latite::getConfigManager().loadMaster()) {
        Logger::Fatal("Could not load master config!");
    }
    else {
        Logger::Info("Loaded master config");
    }

    Logger::Info("Initialized Latite Client");
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
        Latite::getRenderer().~Renderer();
        Latite::getAssets().~Assets();
        Latite::getScreenManager().~ScreenManager();
        Latite::getScriptManager().~ScriptManager();
        Latite::get().~Latite();
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}

Latite& Latite::get() noexcept {
    return *std::launder(reinterpret_cast<Latite*>(latiteBuf));
}

ModuleManager& Latite::getModuleManager() noexcept {
    return *std::launder(reinterpret_cast<ModuleManager*>(mmgrBuf));
}

CommandManager& Latite::getCommandManager() noexcept {
    return *std::launder(reinterpret_cast<CommandManager*>(commandMgrBuf));
}

ConfigManager& Latite::getConfigManager() noexcept {
    return *std::launder(reinterpret_cast<ConfigManager*>(configMgrBuf));
}

ClientMessageSink& Latite::getClientMessageSink() noexcept {
    return *std::launder(reinterpret_cast<ClientMessageSink*>(messageSinkBuf));
}

SettingGroup& Latite::getSettings() noexcept {
    return *std::launder(reinterpret_cast<SettingGroup*>(mainSettingGroup));
}

LatiteHooks& Latite::getHooks() noexcept {
    return *std::launder(reinterpret_cast<LatiteHooks*>(hooks));
}

Eventing& Latite::getEventing() noexcept {
    return *std::launder(reinterpret_cast<Eventing*>(eventing));
}

Renderer& Latite::getRenderer() noexcept {
    return *std::launder(reinterpret_cast<Renderer*>(rendererBuf));
}

ScreenManager& Latite::getScreenManager() noexcept {
    return *std::launder(reinterpret_cast<ScreenManager*>(scnMgrBuf));
}

Assets& Latite::getAssets() noexcept {
    return *std::launder(reinterpret_cast<Assets*>(assetsBuf));
}

ScriptManager& Latite::getScriptManager() noexcept {
    return *std::launder(reinterpret_cast<ScriptManager*>(scriptMgrBuf));
}

void Latite::doEject() noexcept {
    Latite::get().getHooks().uninit();

    FreeLibrary(this->dllInst);
}

void Latite::queueEject() noexcept {
    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    app.Title(L"");
    this->shouldEject = true;
    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, dllInst, 0, nullptr));
}

void Latite::initialize(HINSTANCE hInst) {
    this->dllInst = hInst;
    getHooks().init();
    Logger::Info("Initialized Hooks");
    getHooks().enable();
    Logger::Info("Enabled Hooks");

    initSettings();

    Latite::getScriptManager().init();
    Logger::Info("Script manager initialized.");

    // TODO: use UpdateEvent
    Latite::getEventing().listen<RenderGameEvent>(this, (EventListenerFunc)&Latite::onUpdate, 1);
    Latite::getEventing().listen<KeyUpdateEvent>(this, (EventListenerFunc)&Latite::onKey, 1);
    Latite::getEventing().listen<RendererInitEvent>(this, (EventListenerFunc)&Latite::onRendererInit, 1);
    Latite::getEventing().listen<FocusLostEvent>(this, (EventListenerFunc)&Latite::onFocusLost, 1);
    Latite::getEventing().listen<AppSuspendedEvent>(this, (EventListenerFunc)&Latite::onSuspended, 1);
}

void Latite::threadsafeInit() {
    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    std::string vstr(this->version);
    auto ws = util::StrToWStr("Latite Client " + vstr);
    app.Title(ws);
    Latite::getScriptManager().loadPrerunScripts();
    Logger::Info("Loaded startup scripts");   

    if (!Chakra::mod) {
        winrt::hstring title = L"Error";
        winrt::hstring content = L"Assets\\ChakraCore.dll could not be found, you will not be able to use scripting.";
        winrt::Windows::UI::Popups::MessageDialog dialog(content, title);
        dialog.ShowAsync();
    }
}

void Latite::initSettings() {
    {
        auto set = std::make_shared<Setting>("menuKey", "Menu Key", "The key used to open the menu", Setting::Type::Key);
        set->value = &this->menuKey;
        this->getSettings().addSetting(set);
    }
}

void Latite::onUpdate(Event&) {
    if (!hasInit) {
        threadsafeInit();
        hasInit = true;
    }
    Latite::getScriptManager().runScriptingOperations();
}

void Latite::onKey(Event& evGeneric) {
    auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
    if (ev.getKey() == VK_END && ev.isDown()) {
        this->queueEject();
        Logger::Info("Uninject key pressed");

        return;
    }

    if (ev.isDown() && ev.getKey() == VK_ESCAPE && Latite::getScreenManager().getActiveScreen()) {
        Latite::getScreenManager().exitCurrentScreen();
        ev.setCancelled(true);
        return;
    }

    if ((ev.getKey() == std::get<KeyValue>(this->menuKey)) && ev.isDown()) {
        
        if (!ev.inUI() || Latite::getScreenManager().getActiveScreen()) {
            Latite::getScreenManager().tryToggleScreen("ClickGUI");
            ev.setCancelled(true);
            return;
        }
    }
}

void Latite::onRendererInit(Event&) {
    getAssets().unloadAll(); // should be safe even if we didn't load resources yet
    getAssets().loadAll();
}

void Latite::onFocusLost(Event& evGeneric) {
    auto& ev = reinterpret_cast<FocusLostEvent&>(evGeneric);
    if (Latite::getScreenManager().getActiveScreen()) ev.setCancelled(true);
}

void Latite::onSuspended(Event& ev) {
    Latite::getConfigManager().saveCurrentConfig();
    Logger::Info("Saved config");
}

void Latite::loadConfig(SettingGroup& gr) {
    gr.forEach([&](std::shared_ptr<Setting> set) {
        this->getSettings().forEach([&](std::shared_ptr<Setting> modSet) {
            if (modSet->name() == set->name()) {
                switch (set->type) {
                case Setting::Type::Bool:
                    *modSet->value = std::get<BoolValue>(set->resolvedValue);
                    break;
                case Setting::Type::Int:
                    *modSet->value = std::get<IntValue>(set->resolvedValue);
                    break;
                case Setting::Type::Float:
                    *modSet->value = std::get<FloatValue>(set->resolvedValue);
                    break;
                case Setting::Type::Key:
                    *modSet->value = std::get<KeyValue>(set->resolvedValue);
                    break;
                }
            }
            });
        });
}
