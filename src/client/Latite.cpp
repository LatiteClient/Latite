/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

﻿// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"
#include "pch.h"
#include "util/Util.h"
#include "util/Logger.h"

#include "client/ui/TextBox.h"

#include "feature/module/ModuleManager.h"
#include "feature/command/CommandManager.h"
#include "script/ScriptManager.h"

#include "config/ConfigManager.h"
#include "misc/ClientMessageSink.h"
#include "input/Keyboard.h"
#include "hook/Hooks.h"
#include "event/Eventing.h"
#include "event/impl/KeyUpdateEvent.h"
#include "event/impl/RendererInitEvent.h"
#include "event/impl/RendererCleanupEvent.h"
#include "event/impl/FocusLostEvent.h"
#include "event/impl/AppSuspendedEvent.h"
#include "event/impl/RenderOverlayEvent.h"
#include "event/impl/UpdateEvent.h"
#include "event/impl/CharEvent.h"
#include "event/impl/ClickEvent.h"

#include "sdk/signature/storage.h"

#include "sdk/common/client/game/ClientInstance.h"
#include <winrt/windows.ui.viewmanagement.h>
#include <winrt/Windows.ApplicationModel.Core.h>

#include <winrt/windows.ui.core.h>
#include <winrt/windows.ui.popups.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

#include <winrt/Windows.Web.Http.h>
#include <winrt/impl/windows.web.http.2.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/windows.storage.h>
#include <winrt/windows.storage.streams.h>

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;


#include "misc/AuthWindow.h"
#include "render/Renderer.h"
#include "screen/ScreenManager.h"
#include "render/Assets.h"
#include "resource.h"

#include "xorstr.hpp"

int SDK::internalVers = SDK::VLATEST;

using namespace std;

namespace {
    alignas(Eventing) char eventing[sizeof(Eventing)] = {};
    alignas(Latite) char latiteBuf[sizeof(Latite)] = {};
    alignas(Renderer) char rendererBuf[sizeof(Renderer)] = {};
    alignas(ModuleManager) char mmgrBuf[sizeof(ModuleManager)] = {};
    alignas(ClientMessageSink) char messageSinkBuf[sizeof(ClientMessageSink)] = {};
    alignas(CommandManager) char commandMgrBuf[sizeof(CommandManager)] = {};
    alignas(ConfigManager) char configMgrBuf[sizeof(ConfigManager)] = {};
    alignas(SettingGroup) char mainSettingGroup[sizeof(SettingGroup)] = {};
    alignas(LatiteHooks) char hooks[sizeof(LatiteHooks)] = {};
    alignas(ScreenManager) char scnMgrBuf[sizeof(ScreenManager)] = {};
    alignas(Assets) char assetsBuf[sizeof(Assets)] = {};
    alignas(ScriptManager) char scriptMgrBuf[sizeof(ScriptManager)] = {};
    alignas(Keyboard) char keyboardBuf[sizeof(Keyboard)] = {};

    bool hasInjected = false;
}

#define MVSIG(...) ([]() -> std::pair<SigImpl*, SigImpl*> {\
if (SDK::internalVers == SDK::VLATEST) return {&Signatures::__VA_ARGS__, &Signatures::__VA_ARGS__};\
else { if (SDK::internalVers == SDK::V1_19_51) { return {&Signatures_1_19_51::__VA_ARGS__, &Signatures::__VA_ARGS__}; }return {&Signatures_1_18_12::__VA_ARGS__, &Signatures::__VA_ARGS__}; }\
})()

namespace {
    AuthWindow* wnd = nullptr;
}

DWORD __stdcall startThread(HINSTANCE dll) {
    // Needed for Logger
    new (messageSinkBuf) ClientMessageSink;
    new (eventing) Eventing();
    new (latiteBuf) Latite;

    std::filesystem::create_directory(util::GetLatitePath());
    std::filesystem::create_directory(util::GetLatitePath() / "Assets");
    Logger::Setup();

    Logger::Info("Loading assets");
    Latite::get().dllInst = dll;
    // ... init assets
    Latite::get().initAsset(ICON_LOGO, L"logo.png");
    Latite::get().initAsset(ICON_SEARCH, L"searchicon.png");
    Latite::get().initAsset(ICON_ARROW, L"arrow.png");
    Latite::get().initAsset(ICON_X, L"x.png");
    Latite::get().initAsset(ICON_HUDEDIT, L"hudedit.png");
    Latite::get().initAsset(ICON_ARROWBACK, L"arrow_back.png");
    Latite::get().initAsset(ICON_COG, L"cog.png");
    Latite::get().initAsset(ICON_CHECKMARK, L"checkmark.png");
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

    std::unordered_map<std::string, SDK::Version> versNumMap = {
        { "1.20.15", SDK::VLATEST },
        { "1.20.12", SDK::VLATEST },
        { "1.20.10", SDK::VLATEST },
        { "1.19.50", SDK::V1_19_51 },
        { "1.19.51", SDK::V1_19_51 },
        { "1.18.12", SDK::V1_18_12 },
        { "1.18.10", SDK::V1_18_12 },
        { "1.16.40", SDK::V1_18_12 }
    };

    if (versNumMap.contains(gameVersion)) {
        auto vers =  versNumMap[gameVersion];
        SDK::internalVers = vers;
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
        MVSIG(Components::moveInputComponent),
        MVSIG(Options_getGamma),
        MVSIG(ClientInstance_grabCursor),
        MVSIG(ClientInstance_releaseCursor),
        MVSIG(Level_tick),
        MVSIG(ChatScreenController_sendChatMessage),
        MVSIG(GameRenderer__renderCurrentFrame),
        MVSIG(onClick),
        MVSIG(AppPlatform__fireAppFocusLost),
        MVSIG(MinecraftGame_onAppSuspended),
        MVSIG(RenderController_getOverlayColor),
        MVSIG(ScreenView_setupAndRender),
        MVSIG(KeyMap),
        MVSIG(MinecraftGame__update),
        MVSIG(RakNetConnector_tick),
        MVSIG(GpuInfo),
        MVSIG(RakPeer_GetAveragePing),
        MVSIG(MoveInputHandler_tick),
        MVSIG(MovePlayer),
        MVSIG(LocalPlayer_applyTurnDelta),
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
    new (keyboardBuf) Keyboard(reinterpret_cast<int*>(Signatures::KeyMap.result));

    for (auto& entry : sigList) {
        if (!entry.first->mod) continue;
        auto res = entry.first->resolve();
        if (!res) {
#if LATITE_DEBUG
            Logger::Warn("Signature {} failed to resolve! Pattern: {}", entry.first->name, entry.first->signature);
#endif
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

    // TODO: latite beta only
    wnd = new AuthWindow(Latite::get().dllInst);

    wnd->show();
    wnd->runMessagePump();

    wnd->destroy();

    Logger::Info("Waiting for game to load..");

    while (!SDK::ClientInstance::get()) {
        std::this_thread::sleep_for(10ms);
    }

    Latite::get().initialize(dll);

    // Wait for DX Hook

    Logger::Info("Waiting for DX init..");

    while (!Latite::getRenderer().hasInitialized()) { // hacky
        std::this_thread::sleep_for(10ms);
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
        if (hasInjected) {
            FreeLibrary(hinstDLL);
            return TRUE;
        }

        hasInjected = true;

        DisableThreadLibraryCalls(hinstDLL);
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startThread, hinstDLL, 0, nullptr));
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        delete wnd;
        // Remove singletons

        Latite::getHooks().disable();

        // Wait for hooks to disable
        std::this_thread::sleep_for(200ms);

        Latite::getConfigManager().saveCurrentConfig();

        Latite::getKeyboard().~Keyboard();
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

Keyboard& Latite::getKeyboard() noexcept {
    return *std::launder(reinterpret_cast<Keyboard*>(keyboardBuf));
}

std::optional<float> Latite::getMenuBlur() {
    if (std::get<BoolValue>(this->menuBlurEnabled)) {
        return std::get<FloatValue>(this->menuBlur);
    }
    return std::nullopt;
}

void Latite::queueEject() noexcept {
    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    app.Title(L"");
    this->shouldEject = true;
    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, dllInst, 0, nullptr));
}

void Latite::initialize(HINSTANCE hInst) {
    this->dllInst = hInst;

    initSettings();

    getHooks().init();
    Logger::Info("Initialized Hooks");
    getHooks().enable();
    Logger::Info("Enabled Hooks");

    Latite::getScriptManager().init();
    Logger::Info("Script manager initialized.");

    // TODO: use UpdateEvent
    Latite::getEventing().listen<UpdateEvent>(this, (EventListenerFunc)&Latite::onUpdate, 2);
    Latite::getEventing().listen<KeyUpdateEvent>(this, (EventListenerFunc)&Latite::onKey, 2);
    Latite::getEventing().listen<RendererInitEvent>(this, (EventListenerFunc)&Latite::onRendererInit, 2);
    Latite::getEventing().listen<RendererCleanupEvent>(this, (EventListenerFunc)&Latite::onRendererCleanup, 2);
    Latite::getEventing().listen<FocusLostEvent>(this, (EventListenerFunc)&Latite::onFocusLost, 2);
    Latite::getEventing().listen<AppSuspendedEvent>(this, (EventListenerFunc)&Latite::onSuspended, 2);
    Latite::getEventing().listen<CharEvent>(this, (EventListenerFunc)&Latite::onChar, 2);
    Latite::getEventing().listen<ClickEvent>(this, (EventListenerFunc)&Latite::onClick, 2);
}

void Latite::threadsafeInit() {

    // TODO: latite beta only
    //if (SDK::ClientInstance::get()->minecraftGame->xuid.size() > 0) wnd->postXUID();

    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    std::string vstr(this->version);
    auto ws = util::StrToWStr("Latite Client " + vstr);
    app.Title(ws);
    Latite::getScriptManager().loadPrerunScripts();
    Logger::Info("Loaded startup scripts");

    if (!Latite::getConfigManager().loadMaster()) {
        Logger::Fatal("Could not load master config!");
    }
    else {
        Logger::Info("Loaded master config");
    }

    Latite::getCommandManager().prefix = Latite::get().getCommandPrefix();

    if (!Chakra::mod) {
        winrt::hstring title = L"Error";
        winrt::hstring content = L"Assets\\ChakraCore.dll could not be found, you will not be able to use scripting.";
        winrt::Windows::UI::Popups::MessageDialog dialog(content, title);
        dialog.ShowAsync();
    }
}

void Latite::initSettings() {
    {
        auto set = std::make_shared<Setting>("menuKey", "Menu Key", "The key used to open the menu");
        set->value = &this->menuKey;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("menuBlurEnabled", "Menu Blur", "Whether blur is enabled or disabled for the menu");
        set->value = &this->menuBlurEnabled;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("useDX11", "Use DX11 (+FPS)", "Possible game FPS/Memory boost. Restart if you disable it");
        set->value = &this->useDX11;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("commandPrefix", "Command Prefix", "");
        set->value = &this->commandPrefix;
        set->visible = false;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("menuIntensity", "Blur Intensity", "The intensity of the menu blur");
        set->value = &this->menuBlur;
        set->min = FloatValue(1.f);
        set->max = FloatValue(30.f);
        set->interval = FloatValue(1.f);
        this->getSettings().addSetting(set);
    }
}

void Latite::initAsset(int resource, std::wstring const& filename) {
    HRSRC hRes = FindResource((HMODULE)dllInst, MAKEINTRESOURCE(resource), RT_RCDATA);
    HGLOBAL hData = LoadResource((HMODULE)dllInst, hRes);
    DWORD hSize = SizeofResource((HMODULE)dllInst, hRes);
    char* hFinal = (char*)LockResource(hData);

    auto path = util::GetLatitePath() / "Assets";
    std::filesystem::create_directory(path);

    auto fullPath = path / filename;

    auto ofs = std::ofstream(fullPath.c_str(), std::ios::binary);
    ofs << std::string(hFinal, hSize);
    ofs.close();
}

std::string Latite::getTextAsset(int resource) {
    HRSRC hRes = FindResource((HMODULE)dllInst, MAKEINTRESOURCE(resource), MAKEINTRESOURCE(TEXTFILE));
    HGLOBAL hData = LoadResource((HMODULE)dllInst, hRes);
    DWORD hSize = SizeofResource((HMODULE)dllInst, hRes);
    char* hFinal = (char*)LockResource(hData);

    return std::string(hFinal, hSize);
}

winrt::Windows::Foundation::IAsyncAction Latite::downloadExtraAssets() {
    auto http = HttpClient();

    auto folderPath = util::GetLatitePath() / "Assets";

    // TODO: FIXME: xor
    winrt::Windows::Foundation::Uri requestUri(util::StrToWStr(xorstr_("https://raw.githubusercontent.com/Imrglop/Latite-Releases/main/bin/ChakraCore.dll")));

    auto buffer = http.GetBufferAsync(requestUri).get();
    
    auto folder = co_await StorageFolder::GetFolderFromPathAsync(folderPath.wstring());
    auto file = co_await folder.CreateFileAsync(L"ChakraCore.dll", CreationCollisionOption::OpenIfExists);

    IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);

    DataWriter writer(stream);
    writer.WriteBuffer(buffer);
    writer.StoreAsync().get();
    writer.FlushAsync().get();
    co_return;
}

void Latite::onUpdate(Event& evGeneric) {
    auto& ev = reinterpret_cast<UpdateEvent&>(evGeneric);
    timings.update();

    if (!hasInit) {
        threadsafeInit();
        hasInit = true;
    }
    getKeyboard().findTextInput();
    Latite::getScriptManager().runScriptingOperations();

    static bool lastDX11 = std::get<BoolValue>(this->useDX11);
    if (std::get<BoolValue>(useDX11) != lastDX11) {
        
        if (lastDX11) {
            Latite::getClientMessageSink().display(util::Format("&7Please restart your game to use DX12 again!"));
        }
        else {
            Latite::getRenderer().setShouldReinit();
        }
        lastDX11 = std::get<BoolValue>(useDX11);
    }
}

void Latite::onKey(Event& evGeneric) {
    auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
    if (ev.getKey() == std::get<KeyValue>(ejectKey) && ev.isDown()) {
        this->queueEject();
        Logger::Info("Uninject key pressed");

        return;
    }

    if (ev.isDown()) {
        for (auto& tb : textBoxes) {
            if (tb->isSelected()) {
                tb->onKeyDown(ev.getKey());
            }
        }
    }
}

void Latite::onClick(Event& evGeneric) {
    auto& ev = reinterpret_cast<ClickEvent&>(evGeneric);
    timings.onClick(ev.getMouseButton(), ev.isDown());
}

void Latite::onChar(Event& evGeneric) {
    auto& ev = reinterpret_cast<CharEvent&>(evGeneric);
    for (auto tb : textBoxes) {
        if (tb->isSelected()) {
            if (ev.isChar()) {
                tb->onChar(ev.getChar());
            }
            else {
                auto ch = ev.getChar();
                switch (ch) {
                case 0x1:
                    util::SetClipboardText(tb->getText());
                    break;
                case 0x2:
                    tb->setSelected(false);
                    break;
                case 0x3:
                    tb->reset();
                    break;
                }
            }
            ev.setCancelled(true);
        }
    }
}

void Latite::onRendererInit(Event&) {
    getAssets().unloadAll(); // should be safe even if we didn't load resources yet
    getAssets().loadAll();

    this->hudBlurBitmap = getRenderer().copyCurrentBitmap();
    getRenderer().getDeviceContext()->CreateEffect(CLSID_D2D1GaussianBlur, gaussianBlurEffect.GetAddressOf());

    gaussianBlurEffect->SetInput(0, hudBlurBitmap.Get());
    gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, std::get<FloatValue>(this->hudBlurIntensity));
    gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);

    getRenderer().getDeviceContext()->CreateBitmapBrush(hudBlurBitmap.Get(), this->hudBlurBrush.GetAddressOf());
}

void Latite::onRendererCleanup(Event& ev) {
    this->hudBlurBitmap = nullptr;
    this->gaussianBlurEffect = nullptr;
    this->hudBlurBrush = nullptr;
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
                std::visit([&](auto&& obj) {
                    *modSet->value = obj;
                    }, set->resolvedValue);
            }
            });
        });
}

//char* LatiteGetVersionsSupported() {
//    return nullptr;
//}
