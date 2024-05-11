#include "pch.h"
// LatiteRecode.cpp : Defines the entry point for the application.
//

#include "Latite.h"

#include "client/ui/TextBox.h"

#include "feature/module/ModuleManager.h"
#include "feature/command/CommandManager.h"
#include "script/PluginManager.h"

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
#include "event/impl/UpdateEvent.h"
#include "event/impl/CharEvent.h"
#include "event/impl/ClickEvent.h"
#include "event/impl/BobMovementEvent.h"
#include "event/impl/LeaveGameEvent.h"

#include "sdk/signature/storage.h"

#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "sdk/common/client/game/FontRepository.h"
#include <winrt/windows.ui.viewmanagement.h>
#include <winrt/windows.storage.streams.h>

#include <sdk/common/client/gui/ScreenView.h>
#include <sdk/common/client/gui/controls/VisualTree.h>
#include <sdk/common/client/gui/controls/UIControl.h>

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
#include "feature/module/impl/game/Freelook.h"

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
    alignas(PluginManager) char scriptMgrBuf[sizeof(PluginManager)] = {};
    alignas(Keyboard) char keyboardBuf[sizeof(Keyboard)] = {};
    alignas(Notifications) char notificaitonsBuf[sizeof(Notifications)] = {};

    bool hasInjected = false;
}

namespace shared {
    std::array<char, 100> serverStatus = {};
}

#define MVSIG(...) ([]() -> std::pair<SigImpl*, SigImpl*> {\
/*if (SDK::internalVers == SDK::VLATEST) */return {&Signatures::__VA_ARGS__, &Signatures::__VA_ARGS__}; }\
/*if (SDK::internalVers == SDK::V1_20_40) { return {&Signatures_1_20_40::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/ \
/*if (SDK::internalVers == SDK::V1_20_30) { return {&Signatures_1_20_30::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/ \
/*if (SDK::internalVers == SDK::V1_19_51) { return {&Signatures_1_19_51::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/ \
/*return {&Signatures_1_18_12::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/\
)()

namespace {
    AuthWindow* wnd = nullptr;
}

DWORD __stdcall startThread(HINSTANCE dll) {
    // Needed for Logger
    new (messageSinkBuf) ClientMessageSink;
    new (eventing) Eventing();
    new (latiteBuf) Latite;
    new (notificaitonsBuf) Notifications;

    std::filesystem::create_directory(util::GetLatitePath());
    std::filesystem::create_directory(util::GetLatitePath() / "Assets");
    Logger::Setup();

    Logger::Info(XOR_STRING("Latite Client {}"), Latite::version);
    winrt::Windows::ApplicationModel::Package package = winrt::Windows::ApplicationModel::Package::Current();
    winrt::Windows::ApplicationModel::PackageVersion version = package.Id().Version();

    {
        std::string rev = std::to_string(version.Build);
        std::string rem = rev.substr(0, rev.size() - 2); // remove 2 digits from end

        int ps = std::stoi(rem);
        std::stringstream ss;
        ss << version.Major << "." << version.Minor << "." << ps;// hacky
        Latite::get().gameVersion = ss.str();
    }
    Logger::Info("Minecraft {}", Latite::get().gameVersion);

    Logger::Info(XOR_STRING("Loading assets"));
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
    Latite::get().initAsset(ICON_LOGOWHITE, L"latitewhite.png");
    //
#if LATITE_DEBUG
    Logger::Info("Resolving signatures..");
#endif

    int sigCount = 0;
    int deadCount = 0;

    std::unordered_map<std::string, SDK::Version> versNumMap = {
        { "1.20.80", SDK::VLATEST },
        //{ "1.20.41", SDK::V1_20_40 },
        //{ "1.20.40", SDK::V1_20_40 },
        //{ "1.20.32", SDK::V1_20_30 },
        //{ "1.20.31", SDK::V1_20_30 },
        //{ "1.20.30", SDK::V1_20_30 },
        //{ "1.19.50", SDK::V1_19_51 },
        //{ "1.19.51", SDK::V1_19_51 },
        //{ "1.18.12", SDK::V1_18_12 },
        //{ "1.18.10", SDK::V1_18_12 },
    };

    if (versNumMap.contains(Latite::get().gameVersion)) {
        // not needed as it will always just be latest
        //auto vers =  versNumMap[Latite::get().gameVersion];
        //SDK::internalVers = vers;
    }
    else {
        std::stringstream ss;
        ss << XOR_STRING("Latite Client does not support your version: ") << Latite::get().gameVersion << XOR_STRING(". Latite only supports the following versions:\n\n");

        for (auto& key : versNumMap) {
            ss << key.first << "\n";
        }

        Logger::Warn(ss.str());
    }

    Logger::Info(XOR_STRING("Minecraft SDK version {}"), SDK::internalVers);

    std::vector<std::pair<SigImpl*, SigImpl*>> sigList = {
        MVSIG(Misc::minecraftGamePointer),
        MVSIG(Misc::clientInstance),
        MVSIG(Keyboard_feed),
        MVSIG(LevelRenderer_renderLevel),
        MVSIG(Offset::LevelRendererPlayer_fovX),
        MVSIG(Offset::LevelRendererPlayer_origin),
        MVSIG(Offset::MinecraftGame_cursorGrabbed),
        MVSIG(Components::moveInputComponent),
        MVSIG(Options_getGamma),
        MVSIG(Options_getPerspective),
        MVSIG(Options_getHideHand),
        MVSIG(Options_getSensitivity),
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
        MVSIG(Vtable::TextPacket),
        MVSIG(Vtable::SetTitlePacket),
        MVSIG(Components::runtimeIDComponent),
        MVSIG(Misc::clickMap),
        MVSIG(CameraViewBob),
        MVSIG(ItemStackBase_getHoverName),
        MVSIG(Vtable::CommandRequestPacket),
        MVSIG(Vtable::Level),
        MVSIG(Misc::uiColorMaterial),
        MVSIG(Tessellator_begin),
        MVSIG(Tessellator_vertex),
        MVSIG(Tessellator_color),
        MVSIG(MeshHelpers_renderMeshImmediately),
        MVSIG(BaseActorRenderContext_BaseActorRenderContext),
        MVSIG(ItemRenderer_renderGuiItemNew),
        MVSIG(BaseAttributeMap_getInstance),
        MVSIG(UIControl_setPosition),
        MVSIG(MinecraftGame_getPrimaryClientInstance),
        MVSIG(Components::actorTypeComponent),
        MVSIG(ActorRenderDispatcher_render),
        MVSIG(LevelRendererPlayer_renderOutlineSelection),
        MVSIG(Components::attributesComponent),
        MVSIG(Dimension_getSkyColor),
        MVSIG(Dimension_getTimeOfDay),
        MVSIG(Weather_tick),
        MVSIG(Misc::thirdPersonNametag),
        MVSIG(ItemStackBase_getDamageValue),
        MVSIG(MinecraftPackets_createPacket),
        MVSIG(GameMode_attack),
        MVSIG(GuiData__addMessage),
        MVSIG(Components::actorEquipmentPersistentComponent),
        MVSIG(_updatePlayer),
    };
    
    new (mmgrBuf) ModuleManager;
    new (commandMgrBuf) CommandManager;
    new (mainSettingGroup) SettingGroup("global");
    new (scnMgrBuf) ScreenManager(); // needs to be before renderer
    new (configMgrBuf) ConfigManager();

    new (scriptMgrBuf) PluginManager();
    new (rendererBuf) Renderer();
    new (assetsBuf) Assets();

    for (auto& entry : sigList) {
        if (!entry.first->mod) continue;
        auto res = entry.first->resolve();
        if (!res) {
#if LATITE_DEBUG
            Logger::Warn("Signature {} failed to resolve!", entry.first->name);
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

    MH_Initialize();
    new (hooks) LatiteHooks();

    new (keyboardBuf) Keyboard(reinterpret_cast<int*>(Signatures::KeyMap.result));

    Logger::Info(XOR_STRING("Waiting for user"));

#ifdef LATITE_BETA
    {
        wnd = new AuthWindow(Latite::get().dllInst);
        auto autoLogPath = util::GetLatitePath() / XOR_STRING("login.txt");
        if (std::filesystem::exists(autoLogPath)) {
            std::wifstream ifs{autoLogPath};
            if (ifs.good()) {
                wnd->beforeAuth();
                std::wstring ws;
                std::getline(ifs, ws);
                auto res = wnd->doAuth(ws);
                if (res.empty()) {
                    Logger::Fatal(XOR_STRING("Auth has failed, maybe your token has expired?"));
                }
                wnd->setResult(res);
            }
            else {
                wnd->show();
                wnd->runMessagePump();
                Logger::Fatal("{}", XOR_STRING("Please look at the pinned message in #status in the Latite Discord to setup Beta."));
            }
        }
        else {
            wnd->show();
            wnd->runMessagePump();
            Logger::Fatal("{}", XOR_STRING("Please look at the pinned message in #status in the Latite Discord to setup Beta."));
        }
        wnd->destroy();
    }
#endif

    Logger::Info(XOR_STRING("Waiting for game to load.."));

#ifdef  LATITE_BETA
    // its actually the real offset - 0x10
    Latite::get().cInstOffs2 = wnd->getResult()[3];
    Latite::get().cInstOffs = wnd->getResult()[2];
    Latite::get().plrOffs = wnd->getResult()[0];
    Latite::get().plrOffs2 = wnd->getResult()[1];
#endif

    while (!SDK::ClientInstance::get()) {
        std::this_thread::sleep_for(10ms);
    }

    Latite::get().initSettings();

    Latite::get().initialize(dll);

    Logger::Info(XOR_STRING("Initialized Latite Client"));
    return 0ul;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID)  // reserved
{
    if (GetModuleHandleA("Minecraft.Windows.exe") != GetModuleHandleA(NULL)) return TRUE;

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
#ifdef LATITE_BETA
        delete wnd;
#endif
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
        Latite::getPluginManager().~PluginManager();
        Latite::getNotifications().~Notifications();
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

PluginManager& Latite::getPluginManager() noexcept {
    return *std::launder(reinterpret_cast<PluginManager*>(scriptMgrBuf));
}

Keyboard& Latite::getKeyboard() noexcept {
    return *std::launder(reinterpret_cast<Keyboard*>(keyboardBuf));
}

Notifications& Latite::getNotifications() noexcept {
    return *std::launder(reinterpret_cast<Notifications*>(notificaitonsBuf));
}

std::optional<float> Latite::getMenuBlur() {
    if (std::get<BoolValue>(this->menuBlurEnabled)) {
        return std::get<FloatValue>(this->menuBlur);
    }
    return std::nullopt;
}

std::vector<std::string> Latite::getLatiteUsers() {
    return latiteUsers;
}

void Latite::queueEject() noexcept {
    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    app.Title(L"");
    this->shouldEject = true;
    CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibraryAndExitThread, dllInst, 0, nullptr));
}

SDK::Font* Latite::getFont() {
    switch (this->mcRendFont.getSelectedKey()) {
    case 0:
        return SDK::ClientInstance::get()->minecraftGame->minecraftFont;
    case 1:
        return SDK::ClientInstance::get()->minecraftGame->getFontRepository()->getSmoothFont();
    default:
        Logger::Fatal(XOR_STRING("Unknown font selected: {}"), this->mcRendFont.getSelectedKey());
        throw std::runtime_error(XOR_STRING("Unknown font"));
    }
}

void Latite::initialize(HINSTANCE hInst) {
    this->dllInst = hInst;

    Latite::getEventing().listen<UpdateEvent>(this, (EventListenerFunc)&Latite::onUpdate, 2);
    Latite::getEventing().listen<KeyUpdateEvent>(this, (EventListenerFunc)&Latite::onKey, 2);
    Latite::getEventing().listen<RendererInitEvent>(this, (EventListenerFunc)&Latite::onRendererInit, 2);
    Latite::getEventing().listen<RendererCleanupEvent>(this, (EventListenerFunc)&Latite::onRendererCleanup, 2);
    Latite::getEventing().listen<FocusLostEvent>(this, (EventListenerFunc)&Latite::onFocusLost, 2);
    Latite::getEventing().listen<AppSuspendedEvent>(this, (EventListenerFunc)&Latite::onSuspended, 2);
    Latite::getEventing().listen<CharEvent>(this, (EventListenerFunc)&Latite::onChar, 2);
    Latite::getEventing().listen<ClickEvent>(this, (EventListenerFunc)&Latite::onClick, 2);
    Latite::getEventing().listen<BobMovementEvent>(this, (EventListenerFunc)&Latite::onBobView, 2);
    Latite::getEventing().listen<LeaveGameEvent>(this, (EventListenerFunc)&Latite::onLeaveGame, 2);
    Latite::getEventing().listen<RenderLayerEvent>(this, (EventListenerFunc)&Latite::onRenderLayer, 2);
    Latite::getEventing().listen<RenderOverlayEvent>(this, (EventListenerFunc)&Latite::onRenderOverlay, 2);
    //Latite::getEventing().listen<PacketReceiveEvent>(this, (EventListenerFunc)&Latite::onPacketReceive, 2);
    Latite::getEventing().listen<TickEvent>(this, (EventListenerFunc)&Latite::onTick, 2);

    Logger::Info(XOR_STRING("Initialized Hooks"));
    getHooks().enable();
    Logger::Info(XOR_STRING("Enabled Hooks"));

    Latite::getPluginManager().init();
    Logger::Info(XOR_STRING("Script manager initialized."));

    // doesn't work, maybe it's stored somewhere else too
    //if (SDK::internalVers < SDK::V1_20) {
    //    patchKey();
    //}
}

void Latite::threadsafeInit() {
    this->gameThreadId = std::this_thread::get_id();
    // TODO: latite beta only
    //if (SDK::ClientInstance::get()->minecraftGame->xuid.size() > 0) wnd->postXUID();

    auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    std::string vstr(this->version);
    auto ws = util::StrToWStr(XOR_STRING("Latite Client ") + vstr);
    app.Title(ws);
    Latite::getPluginManager().loadPrerunScripts();
    Logger::Info(XOR_STRING("Loaded startup scripts"));
    if (!Latite::getConfigManager().loadMaster()) {
        Logger::Fatal(XOR_STRING("Could not load master config!"));
    }
    else {
        Logger::Info(XOR_STRING("Loaded master config"));
    }
    Latite::getRenderer().setShouldInit();

    Latite::getCommandManager().prefix = Latite::get().getCommandPrefix();

    Latite::getNotifications().push(XW("Welcome to Latite Client!"));
    Latite::getNotifications().push(L"Press " + util::StrToWStr(util::KeyToString(Latite::getMenuKey().value)) + L" in a world or server to open the mod menu.");
}

void Latite::patchKey() {
    // next to "certificateAuthority"
    //                                           MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE8ELkixyLcwlZryUQcu1TvPOmI2B7vX83ndnWRUaXm74wFfa5f/lwQNTfrLVHa2PmenpGI6JhIMUJaWZrjmMj90NoKNFSNBuKdm8rYiXsfaz3K36x/1U26HpG0ZxK/V1V
    static constexpr std::string_view old_key = "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE8ELkixyLcwlZryUQcu1TvPOmI2B7vX83ndnWRUaXm74wFfa5f/lwQNTfrLVHa2PmenpGI6JhIMUJaWZrjmMj90NoKNFSNBuKdm8rYiXsfaz3K36x/1U26HpG0ZxK/V1V";
    static constexpr std::string_view new_key = "MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAECRXueJeTDqNRRgJi/vlRufByu/2G0i2Ebt6YMar5QX/R0DIIyrJMcUpruK4QveTfJSTp3Shlq4Gk34cD/4GUWwkv0DVuzeuB+tXija7HBxii03NHDbPAD0AKnLr2wdAp";

    auto str = memory::findString(old_key, "Minecraft.Windows.exe");
    if (!str) {
        Logger::Info("No old key found");
        return;
    }

    DWORD oProt;
    VirtualProtect(str, old_key.size(), PAGE_EXECUTE_READWRITE, &oProt);
    memcpy(str, new_key.data(), new_key.size());
    VirtualProtect(str, old_key.size(), oProt, &oProt);

    Logger::Info("Old and new keys patched");
}


static void blockModules(std::string_view moduleName, std::string_view serverName) {
    auto inst = SDK::RakNetConnector::get();

    std::vector<std::string> blockedList;
    if (inst->dns.find(serverName) != std::string::npos) {
        Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> mod) {
            if (!mod->isBlocked()) {
                if (mod->name() == moduleName) {
                    blockedList.push_back(mod->getDisplayName());
                    mod->setBlocked(true);
                }
            }
            });
    }

    if (!blockedList.empty()) {
        std::wstring str;
        for (size_t i = 0; i < blockedList.size(); i++) {
            str += util::StrToWStr(blockedList[i]);
            if (i != blockedList.size() - 1) {
                str += L", ";
            }
        }
        str += L" will be blocked on this server.";

        Latite::getNotifications().push(str);
    }
}

void Latite::updateModuleBlocking() {
    auto inst = SDK::RakNetConnector::get();
    if (!inst) return;


    if (inst->dns.size() > 0) {
        // scuffed but we don't have a proper static management system

        static_assert(std::is_base_of_v<Module, Freelook>);

        blockModules("Freelook", "hivebedrock");
        blockModules("Freelook", "nethergames");
        blockModules("Freelook", "galaxite");
    }
    else {
        
    }
}

namespace {
    HttpClient client{};
}

void Latite::fetchLatiteUsers() {
    auto lp = SDK::ClientInstance::get()->getLocalPlayer();
    if (!lp) {
        latiteUsers = {};
        return;
    }

    std::string str = XOR_STRING("https://lafa-1-f2031735.deta.app/users");

    winrt::Windows::Foundation::Uri requestUri(util::StrToWStr(str));

    auto name = util::StrToWStr(lp->playerName);

    auto content = HttpStringContent(util::StrToWStr(XOR_STRING("{\"name\":\"")) + name + util::StrToWStr(XOR_STRING("\"}")));
    std::string medType = XOR_STRING("application/json");
    content.Headers().ContentType().MediaType(util::StrToWStr(medType));
    auto usersDirty = &this->latiteUsersDirty;
    client.PostAsync(requestUri, content).Completed([usersDirty](winrt::Windows::Foundation::IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress> task, winrt::Windows::Foundation::AsyncStatus status) {
        if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
            try {
                auto res = task.GetResults();
                if (res.IsSuccessStatusCode()) {
                    auto cont = res.Content();
                    auto str = cont.ReadAsStringAsync().get();

                    nlohmann::json json;
                    try {

                        json = nlohmann::json::parse(util::WStrToStr(str.c_str()));
                    }
                    catch (nlohmann::json::parse_error&) {
                        usersDirty->clear();
                        for (auto& item : json) {
                            usersDirty->push_back(item.get<std::string>());
                        }
                    }
                }
            }
            catch (winrt::hresult_error&) {
            }
        }
        });
}

void Latite::initSettings() {
    {
        auto set = std::make_shared<Setting>("menuKey", "Menu Key", "The key used to open the menu");
        set->value = &this->menuKey;
        set->callback = [this](Setting& set) {
            Latite::getScreenManager().get<HUDEditor>().key = this->getMenuKey();
        };
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("ejectKey", "Eject Key", "The key used to uninject the client");
        set->value = &this->ejectKey;
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
        auto set = std::make_shared<Setting>("commandPrefix", "Command Prefix", "Command Prefix");
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
    {
        auto set = std::make_shared<Setting>("accentColor", "Accent Color", "Accent Color");
        set->value = &this->accentColor;
        this->getSettings().addSetting(set);
    }

    {
        //auto set = std::make_shared<Setting>("minViewBob", "Minimal View Bob", "Only bob the item in hand, not the camera");
        //set->value = &this->minimalViewBob;
        //this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("minecraftRenderer", "Use Minecraft Renderer", "Use the Minecraft renderer in the HUD.");
        set->value = &this->minecraftRenderer;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("textShadow", "Text Shadows", "Whether to have text shadows or not with Minecraft renderer.", "minecraftRenderer"_istrue);
        set->value = &this->textShadow;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("mcRendererFont", "HUD Font", "The HUD font", "minecraftRenderer"_istrue);
        set->enumData = &this->mcRendFont;
        set->value = set->enumData->getValue();
        set->enumData->addEntry({ 0, "Default", "The Minecraft font" });
        set->enumData->addEntry({ 1, "Noto Sans", "The smooth font (Noto Sans MS)" });
        this->getSettings().addSetting(set);
    }

    {
        //auto set = std::make_shared<Setting>("broadcastClientUsage", "Latite Client Presence", "If you leave this on, others with Latite will see that you are using Latite and you will see other people who use Latite.");
        //set->value = &this->broadcastUsage;
        //this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("centerCursor", "Center cursor when opening UIs", "Crosshair will be centered when you open the menu.");
        set->value = &this->centerCursorMenus;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("snapLines", "Snap Lines", "Snap lines to help you align modules");
        set->value = &this->snapLines;
        this->getSettings().addSetting(set);
    }
}

void Latite::queueForUIRender(std::function<void(SDK::MinecraftUIRenderContext* ctx)> callback) {
    this->uiRenderQueue.push(callback);
}

void Latite::queueForClientThread(std::function<void()> callback) {
    this->clientThreadQueue.push(callback);
}

void Latite::queueForDXRender(std::function<void(ID2D1DeviceContext* ctx)> callback) {
    this->dxRenderQueue.push(callback);
}

void Latite::initAsset(int resource, std::wstring const& filename) {
#ifdef DEBUG
    Logger::Info("Getting asset: {} ({})", util::WStrToStr(filename), resource);
#endif

    HRSRC hRes = FindResource((HMODULE)dllInst, MAKEINTRESOURCE(resource), RT_RCDATA);
    if (!hRes) {
        Logger::Fatal(XOR_STRING("Could not find resource {}"), util::WStrToStr(filename));
        winrt::terminate();
        return;
    }

    HGLOBAL hData = LoadResource((HMODULE)dllInst, hRes);
    DWORD hSize = SizeofResource((HMODULE)dllInst, hRes);
    char* hFinal = (char*)LockResource(hData);

    auto path = util::GetLatitePath() / XOR_STRING("Assets");
    std::filesystem::create_directory(path);

    auto fullPath = path / filename;

    auto ofs = std::ofstream(fullPath.c_str(), std::ios::binary);
    ofs << std::string(hFinal, hSize);
    ofs.flush();
}

std::string Latite::getTextAsset(int resource) {
    HRSRC hRes = FindResource((HMODULE)dllInst, MAKEINTRESOURCE(resource), MAKEINTRESOURCE(TEXTFILE));
    if (!hRes) {
        Logger::Fatal("Could not find text resource {}", resource);
        throw std::runtime_error("Could not find resource");
    }

    HGLOBAL hData = LoadResource((HMODULE)dllInst, hRes);
    DWORD hSize = SizeofResource((HMODULE)dllInst, hRes);
    char* hFinal = (char*)LockResource(hData);

    return std::string(hFinal, hSize);
}

namespace {
    winrt::Windows::Foundation::IAsyncAction doDownloadAssets() {
        auto http = HttpClient();

        auto folderPath = util::GetLatitePath() / "Assets";

        winrt::Windows::Foundation::Uri requestUri(util::StrToWStr(XOR_STRING("https://raw.githubusercontent.com/Imrglop/Latite-Releases/main/bin/ChakraCore.dll")));

        auto buffer = co_await http.GetBufferAsync(requestUri);

        std::filesystem::create_directories(folderPath);

        auto folder = co_await StorageFolder::GetFolderFromPathAsync(folderPath.wstring());
        auto file = co_await folder.CreateFileAsync(L"ChakraCore.dll", CreationCollisionOption::OpenIfExists);
        IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);

        DataWriter writer(stream);
        writer.WriteBuffer(buffer);
        writer.StoreAsync().get();
        writer.FlushAsync().get();
        co_return;
    }
}

void Latite::downloadChakraCore() {
    if (!downloadingAssets) {
        this->downloadingAssets = true;
        doDownloadAssets();
    }
}

void Latite::onUpdate(Event& evGeneric) {
    auto& ev = reinterpret_cast<UpdateEvent&>(evGeneric);
    timings.update();
    auto now = std::chrono::system_clock::now();
    static auto lastSend = now;

    while (!this->clientThreadQueue.empty()) {
        auto& latest = this->clientThreadQueue.front();
        latest();
        this->clientThreadQueue.pop();
    }

    auto rak = SDK::RakNetConnector::get();

    if (!rak || rak->ipAddress.empty()) {
        //updateModuleBlocking();
        getModuleManager().forEach([](std::shared_ptr<IModule> mod) {
            mod->setBlocked(false);
            });
    }

    bool grabbed = SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed();
    static bool lastGrabbed = grabbed;

    if (!minecraftWindow) {
        minecraftWindow = FindWindowA(NULL, XOR_STRING("Minecraft"));
    }

    if (std::get<BoolValue>(centerCursorMenus) && grabbed && !lastGrabbed) {
        RECT r = { 0, 0, 0, 0 };
        GetClientRect(minecraftWindow, &r);
        SetCursorPos(r.left + r.right / 2, r.top + r.bottom / 2);
    }
    lastGrabbed = grabbed;
    
    if (std::get<BoolValue>(broadcastUsage)) {
        //if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSend) > 10s) {
        //    this->fetchLatiteUsers();
        //    lastSend = now;
        //}
    }
    
    latiteUsers = latiteUsersDirty;

    if (!hasInit) {
        threadsafeInit();
        hasInit = true;
    }
    getKeyboard().findTextInput();
    Latite::getPluginManager().runScriptingOperations();

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
#if 0
    {
        static auto time = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - time) > 5s) {

            std::string newServerStatus = "menus";
            auto lp = SDK::ClientInstance::get()->getLocalPlayer();
            auto rak = SDK::RakNetConnector::get();

            if (lp) {
                if (rak && !rak->ipAddress.empty()) {
                    newServerStatus = "server";
                    if (!rak->featuredServer.empty()) newServerStatus = "server: " + rak->featuredServer;
                    else if (!rak->dns.empty()) newServerStatus = "server: " + rak->dns + (rak->port == 19132 ? "" : ":" + std::to_string(rak->port));
                    // Don't show non-dns ips (1.1.1.1) for privacy
                }
                else {
                    newServerStatus = "world: " + SDK::ClientInstance::get()->minecraft->getLevel()->name;
                }
            }

            if (newServerStatus.size() > shared::serverStatus.max_size()) { // make sure there are no buffer overflows
                newServerStatus = newServerStatus.substr(0, shared::serverStatus.max_size() - 2);
            }
            strcpy_s(shared::serverStatus.data(), shared::serverStatus.max_size(), newServerStatus.c_str());
            time = now;
        }
    }
#endif
}

void Latite::onKey(Event& evGeneric) {
    auto& ev = reinterpret_cast<KeyUpdateEvent&>(evGeneric);
    if (ev.getKey() == std::get<KeyValue>(ejectKey) && ev.isDown()) {
        this->queueEject();
        Logger::Info("Uninject key pressed");

        ev.setCancelled();
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

    this->hudBlurBitmap = getRenderer().getCopiedBitmap();
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
    Logger::Info(XOR_STRING("Saved config"));
}

void Latite::onBobView(Event& ev) {
    if (std::get<BoolValue>(this->minimalViewBob)) {
        reinterpret_cast<Cancellable&>(ev).setCancelled(true);
    }
}

void Latite::onLeaveGame(Event& ev) {
    getRenderer().clearTextCache();
}

void Latite::onRenderLayer(Event& evG) {
    auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
    while (!this->uiRenderQueue.empty()) {
        auto& latest = this->uiRenderQueue.front();
        latest(ev.getUIRenderContext());
        this->uiRenderQueue.pop();
    }

    if (ev.getScreenView()->visualTree->rootControl->name == XOR_STRING("start_screen")) {
        MCDrawUtil dc{ ev.getUIRenderContext(), SDK::ClientInstance::get()->minecraftGame->minecraftFont };
        
        auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;
        dc.drawText({ 0, 0, ss.x, ss.y }, XW("Latite Client ") + util::StrToWStr(std::string(Latite::version)), d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular, 30.f);
        dc.drawText({ 0, 30, ss.x, ss.y }, XW("latite.net"), d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular, 30.f);
        dc.drawText({ 0, 60, ss.x, ss.y }, XW("github.com/LatiteClient"), d2d::Colors::WHITE, Renderer::FontSelection::SegoeRegular, 30.f);
        dc.flush(true, false);
    }
}

void Latite::onRenderOverlay(Event& evG) {
    auto& ev = reinterpret_cast<RenderOverlayEvent&>(evG);

    while (!this->dxRenderQueue.empty()) {
        auto& latest = this->dxRenderQueue.front();
        latest(ev.getDeviceContext());
        this->dxRenderQueue.pop();
    }
}

void Latite::onPacketReceive(Event& evG) {
    // disabled
    auto& ev = reinterpret_cast<PacketReceiveEvent&>(evG);
}

void Latite::onTick(Event& ev) {
    updateModuleBlocking();
}

void Latite::loadConfig(SettingGroup& gr) {
    gr.forEach([&](std::shared_ptr<Setting> set) {
        this->getSettings().forEach([&](std::shared_ptr<Setting> modSet) {
            if (modSet->name() == set->name()) {
                std::visit([&](auto&& obj) {
                    *modSet->value = obj;
                    modSet->update();
                    }, set->resolvedValue);
            }
            });
        });
}
