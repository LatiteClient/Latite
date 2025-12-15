#include "pch.h"
// LatiteRecode.cpp : Defines the entry point for the application.
//
#include <regex>

#include "Latite.h"
#include "localization/LocalizeString.h"

#include "client/screen/TextBox.h"

#include "feature/module/ModuleManager.h"
#include "feature/command/CommandManager.h"
#include "script/PluginManager.h"

#include "config/ConfigManager.h"
#include "misc/ClientMessageQueue.h"
#include "input/Keyboard.h"
#include "memory/hook/Hooks.h"
#include "event/Eventing.h"
#include "event/events/KeyUpdateEvent.h"
#include "event/events/RendererInitEvent.h"
#include "event/events/RendererCleanupEvent.h"
#include "event/events/FocusLostEvent.h"
#include "event/events/AppSuspendedEvent.h"
#include "event/events/UpdateEvent.h"
#include "event/events/CharEvent.h"
#include "event/events/ClickEvent.h"
#include "event/events/BobMovementEvent.h"
#include "event/events/LeaveGameEvent.h"

#include "mc/Addresses.h"

#include "mc/common/client/game/ClientInstance.h"
#include "mc/common/client/game/MinecraftGame.h"
#include "mc/common/client/game/FontRepository.h"
#include <winrt/windows.ui.viewmanagement.h>
#include <winrt/windows.storage.streams.h>
#include <winrt/windows.security.cryptography.h>
#include <winrt/windows.security.cryptography.core.h>
#include <winrt/windows.system.userprofile.h>
// this looks like its unused, but this provides types for language.DisplayName(). compilation will fail without it.
#include <winrt/windows.globalization.h>

#include <mc/common/client/gui/ScreenView.h>
#include <mc/common/client/gui/controls/VisualTree.h>
#include <mc/common/client/gui/controls/UIControl.h>

#include "mc/common/client/game/GameCore.h"

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;


#include "render/Renderer.h"
#include "screen/ScreenManager.h"
#include "render/asset/Assets.h"
#include "resource/Resource.h"
#include "feature/module/modules/game/Freelook.h"

int SDK::internalVers = SDK::VLATEST;

using namespace std;

namespace {
    alignas(Eventing) char eventing[sizeof(Eventing)] = {};
    alignas(Latite) char latiteBuf[sizeof(Latite)] = {};
    alignas(Renderer) char rendererBuf[sizeof(Renderer)] = {};
    alignas(ModuleManager) char mmgrBuf[sizeof(ModuleManager)] = {};
    alignas(ClientMessageQueue) char messageSinkBuf[sizeof(ClientMessageQueue)] = {};
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

#define MVSIG(...) ([]() -> std::pair<SigImpl*, SigImpl*> {\
/*if (SDK::internalVers == SDK::VLATEST) */return {&Signatures::__VA_ARGS__, &Signatures::__VA_ARGS__}; }\
/*if (SDK::internalVers == SDK::V1_20_40) { return {&Signatures_1_20_40::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/ \
/*if (SDK::internalVers == SDK::V1_20_30) { return {&Signatures_1_20_30::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/ \
/*if (SDK::internalVers == SDK::V1_19_51) { return {&Signatures_1_19_51::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/ \
/*return {&Signatures_1_18_12::__VA_ARGS__, &Signatures::__VA_ARGS__}; }*/\
)()

DWORD __stdcall startThread(HINSTANCE dll) {
    BEGIN_ERROR_HANDLER
    // Needed for Logger
    new (messageSinkBuf) ClientMessageQueue;
    new (eventing) Eventing();
    new (latiteBuf) Latite;
    new (notificaitonsBuf) Notifications;

    std::filesystem::create_directory(util::GetLatitePath());
    std::filesystem::create_directory(util::GetLatitePath() / "Assets");
    Logger::Setup();

#ifdef LATITE_DEBUG
    AddVectoredExceptionHandler(1, VectoredExceptionHandler);
#endif

    Logger::Info(XOR_STRING("Latite Client {}"), Latite::version);

    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);

    DWORD handle;
    DWORD size = GetFileVersionInfoSizeA(path, &handle);

    if (size == 0) {
        Logger::Fatal("Failed to get file version size");
    }

    std::vector<BYTE> data(size);
    if (!GetFileVersionInfoA(path, handle, size, data.data())) {
        Logger::Fatal("Failed to get file version");
    }

    VS_FIXEDFILEINFO* fileInfo = nullptr;
    UINT len = 0;

    if (VerQueryValueA(data.data(), "\\", reinterpret_cast<LPVOID*>(&fileInfo), &len)) {
        const auto major = HIWORD(fileInfo->dwFileVersionMS);
        const auto minor = LOWORD(fileInfo->dwFileVersionMS);
        const auto build = HIWORD(fileInfo->dwFileVersionLS);

        Latite::get().gameVersion = std::format("{}.{}.{}", major, minor, build);
    }
    
    /*winrt::Windows::ApplicationModel::Package package = winrt::Windows::ApplicationModel::Package::Current();
    winrt::Windows::ApplicationModel::PackageVersion version = package.Id().Version();

    {
        std::string rev = std::to_string(version.Build);
        std::string rem = rev.substr(0, rev.size() - 2); // remove 2 digits from end

        int ps = std::stoi(rem);
        std::stringstream ss;
        ss << version.Major << "." << version.Minor << "." << ps;// hacky
        Latite::get().gameVersion = ss.str();
    }*/
    Logger::Info("Minecraft {}", Latite::get().gameVersion);

    Logger::Info(XOR_STRING("Loading assets"));
    Latite::get().dllInst = dll;
    // ... init assets
    Latite::get().initL10n();

    Logger::Info("Resolving signatures..");

    int sigCount = 0;
    int deadCount = 0;

    std::unordered_map<std::string, SDK::Version> versNumMap = {
        { "1.21.130", SDK::V1_21_130 },
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
        MVSIG(MainWindow__windowProcCallback),
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
        MVSIG(GameCore_handleMouseInput),
        MVSIG(MinecraftGame_onDeviceLost),
        MVSIG(MinecraftGame_onAppSuspended),
        MVSIG(RenderController_getOverlayColor),
        MVSIG(ScreenView_setupAndRender),
        MVSIG(KeyMap),
        MVSIG(MinecraftGame__update),
        MVSIG(GpuInfo),
        MVSIG(RakPeer_GetAveragePing),
        MVSIG(MoveInputHandler_tick),
        MVSIG(ClientInputUpdateSystem_tickBaseInput),
        MVSIG(LocalPlayer_applyTurnDelta),
        MVSIG(Vtable::TextPacket),
        MVSIG(Vtable::SetTitlePacket),
        MVSIG(Components::runtimeIDComponent),
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
        MVSIG(UIControl_getPosition),
        MVSIG(MinecraftGame_getPrimaryClientInstance),
        MVSIG(Components::actorTypeComponent),
        MVSIG(ActorRenderDispatcher_render),
        MVSIG(LevelRendererPlayer_renderOutlineSelection),
        MVSIG(Components::attributesComponent),
        MVSIG(Dimension_getSkyColor),
        MVSIG(Dimension_getTimeOfDay),
        MVSIG(Dimension_tick),
        MVSIG(Misc::thirdPersonNametag),
        MVSIG(ItemStackBase_getDamageValue),
        MVSIG(MinecraftPackets_createPacket),
        MVSIG(Actor_attack),
        MVSIG(GuiData__addMessage),
        MVSIG(Components::actorEquipmentPersistentComponent),
        MVSIG(_updatePlayer),
        MVSIG(GameArguments__onUri),
        MVSIG(RenderMaterialGroup__common),
        MVSIG(GuiData_displayClientMessage),
        MVSIG(Misc::gameCorePointer),
        MVSIG(MouseInputVector)
    };
    
    new (configMgrBuf) ConfigManager();
    if (!Latite::getConfigManager().loadMaster()) {
        Logger::Fatal(XOR_STRING("Could not load master config!"));
    }
    else {
        Logger::Info(XOR_STRING("Loaded master config"));
    }
    new (mainSettingGroup) SettingGroup("global");

    // The Language setting is a special case because we need it to apply names to other global settings.
    Latite::get().initLanguageSetting();
    Latite::getConfigManager().applyLanguageConfig("language");

    Latite::get().initSettings();
    Latite::getConfigManager().applyGlobalConfig();

    Latite::get().detectLanguage();

    new (mmgrBuf) ModuleManager;
    new (commandMgrBuf) CommandManager;
    new (scnMgrBuf) ScreenManager(); // needs to be initialized before renderer

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

    Logger::Info(XOR_STRING("Waiting for game to load.."));

    while (!SDK::ClientInstance::get()) {
        std::this_thread::sleep_for(10ms);
    }

    Latite::get().initialize(dll);

    Logger::Info(XOR_STRING("Initialized Latite Client"));
    return 0ul;
    END_ERROR_HANDLER
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID)  // reserved
{
    BEGIN_ERROR_HANDLER
    if (GetModuleHandleA("Minecraft.Windows.exe") != GetModuleHandleA(NULL)) return TRUE;

    if (fdwReason == DLL_PROCESS_ATTACH) {
        hasInjected = true;

        DisableThreadLibraryCalls(hinstDLL);
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startThread, hinstDLL, 0, nullptr));
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        // Remove singletons

        Latite::getHooks().disable();

        // Wait for all running hooks accross different threads to stop executing
        std::this_thread::sleep_for(200ms);

        Latite::getConfigManager().saveCurrentConfig();

        Latite::getKeyboard().~Keyboard();
        Latite::getModuleManager().~ModuleManager();
        Latite::getClientMessageQueue().~ClientMessageQueue();
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

        MH_Uninitialize();

        hasInjected = false;
        Logger::Info("Latite Client detached.");
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
    END_ERROR_HANDLER
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

ClientMessageQueue& Latite::getClientMessageQueue() noexcept {
    return *std::launder(reinterpret_cast<ClientMessageQueue*>(messageSinkBuf));
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
    //auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    //app.Title(L"");
    SetWindowTextW(SDK::GameCore::get()->hwnd, L"Minecraft");
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

    Latite::getPluginManager().init();
    Logger::Info(XOR_STRING("Script manager initialized."));

    Latite::getEventing().listen<UpdateEvent, &Latite::onUpdate>(this, 2);
    Latite::getEventing().listen<KeyUpdateEvent, &Latite::onKey>(this, 2);
    Latite::getEventing().listen<RendererInitEvent, &Latite::onRendererInit>(this, 2);
    Latite::getEventing().listen<RendererCleanupEvent, &Latite::onRendererCleanup>(this, 2);
    Latite::getEventing().listen<AppSuspendedEvent, &Latite::onSuspended>(this, 2);
    Latite::getEventing().listen<CharEvent, &Latite::onChar>(this, 2);
    Latite::getEventing().listen<ClickEvent, &Latite::onClick>(this, 2);
    Latite::getEventing().listen<BobMovementEvent, &Latite::onBobView>(this, 2);
    Latite::getEventing().listen<LeaveGameEvent, &Latite::onLeaveGame>(this, 2);
    Latite::getEventing().listen<RenderLayerEvent, &Latite::onRenderLayer>(this, 2);
    Latite::getEventing().listen<RenderOverlayEvent, &Latite::onRenderOverlay>(this, 2);
    Latite::getEventing().listen<TickEvent, &Latite::onTick>(this, 2);

    Logger::Info(XOR_STRING("Initialized Hooks"));
    getHooks().enable();
    Logger::Info(XOR_STRING("Enabled Hooks"));

    // doesn't work, maybe it's stored somewhere else too
    //if (SDK::internalVers < SDK::V1_20) {
    //    patchKey();
    //}
}

void Latite::threadsafeInit() {
    this->gameThreadId = std::this_thread::get_id();
    // TODO: latite beta only
    //if (SDK::ClientInstance::get()->minecraftGame->xuid.size() > 0) wnd->postXUID();


#ifdef LATITE_DEBUG
    // Set SEH translator for game thread
    _set_se_translator(translate_seh_to_cpp_exception);
#endif


    //auto app = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
    std::string vstr(this->version);
	
#if defined(LATITE_NIGHTLY)
    auto ws = util::StrToWStr("Latite Client [NIGHTLY] " + gameVersion + " " + vstr + "/" + calcCurrentDLLHash());
#elif defined(LATITE_DEBUG)
    auto ws = util::StrToWStr("Latite Client [DEBUG] " + gameVersion + " " + vstr + "/" + calcCurrentDLLHash());
#else
    auto ws = util::StrToWStr("Latite Client " + vstr);
#endif

    //app.Title(ws);
    SetWindowTextW(SDK::GameCore::get()->hwnd, ws.c_str());
    Latite::getPluginManager().loadPrerunScripts();
    Logger::Info(XOR_STRING("Loaded startup scripts"));
    
    Latite::getConfigManager().applyModuleConfig();

    Latite::getRenderer().setShouldInit();

    Latite::getCommandManager().prefix = Latite::get().getCommandPrefix();
    Latite::getNotifications().push(LocalizeString::get("client.intro.welcome"));
    Latite::getNotifications().push(util::FormatWString(LocalizeString::get("client.intro.menubutton"), { util::StrToWStr(util::KeyToString(Latite::get().getMenuKey().value)) }));
}

static void blockModules(std::string_view moduleName, std::string_view serverName) {
    auto inst = SDK::RakNetConnector::get();

    std::vector<std::wstring> blockedList;
    if (inst->dns.find(serverName) != std::string::npos) {
        Latite::getModuleManager().forEach([&](std::shared_ptr<Module> mod) {
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
            str += blockedList[i];
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
        blockModules("Freelook", "galaxite");
    }
    else {
        
    }
}

namespace {
    HttpClient client{};
}


// TODO: Remove this, there's no point of it being here
std::string Latite::fetchLatestGitHash() {
    std::string url = "https://api.github.com/repos/LatiteClient/Latite/commits/master";
    HttpClient client;
    winrt::Windows::Foundation::Uri uri(winrt::to_hstring(url));

    HttpRequestMessage request(HttpMethod::Get(), uri);
    request.Headers().Insert(winrt::to_hstring("User-Agent"), winrt::to_hstring("WinRTApp"));

    auto response = client.SendRequestAsync(request).get();
    if (response.StatusCode() != HttpStatusCode::Ok) {
        return "hash unknown";
    }

    auto jsonResponse = winrt::to_string(response.Content().ReadAsStringAsync().get());

    auto json = nlohmann::json::parse(jsonResponse);
    return json["sha"];
}

// Also remove this
std::string Latite::calcCurrentDLLHash() {
    std::wstring dllPath = Latite::get().GetCurrentModuleFilePath(dllInst);

    std::ifstream file(dllPath, std::ios::binary);
    if (!file.is_open()) {
        return "Failed to open file";
    }

    std::vector<char> fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::string header = "blob " + std::to_string(fileContent.size()) + '\0';

    std::vector<unsigned char> data(header.begin(), header.end());
    data.insert(data.end(), fileContent.begin(), fileContent.end());

    IBuffer inputBuffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::CreateFromByteArray(data);

    winrt::Windows::Security::Cryptography::Core::HashAlgorithmProvider sha1Provider =
        winrt::Windows::Security::Cryptography::Core::HashAlgorithmProvider::OpenAlgorithm(L"SHA1");

    IBuffer hashBuffer = sha1Provider.HashData(inputBuffer);

    if (hashBuffer.Length() != sha1Provider.HashLength()) {
        return "Hash computation failed";
    }

    winrt::com_array<uint8_t> hashData;
    winrt::Windows::Security::Cryptography::CryptographicBuffer::CopyToByteArray(hashBuffer, hashData);

    std::ostringstream hashString;
    for (unsigned char byte : hashData) {
        hashString << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte);
    }

    return hashString.str().substr(0, 7);
}

std::wstring Latite::GetCurrentModuleFilePath(HMODULE hModule) {
    std::vector<wchar_t> buffer(MAX_PATH);

    DWORD result = GetModuleFileNameW(hModule, buffer.data(), static_cast<DWORD>(buffer.size()));

    if (result > 0 && result < buffer.size()) {
        return std::wstring(buffer.data());
    }
    else if (result >= buffer.size()) {
        buffer.resize(result + 1);
        result = GetModuleFileNameW(hModule, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (result > 0 && result < buffer.size()) {
            return std::wstring(buffer.data());
        }
    }

    return std::wstring(L"couldn't get file path");
}

void Latite::initSettings() {
    {
        auto set = std::make_shared<Setting>("menuKey", LocalizeString::get("client.settings.menuKey.name"),
                                             LocalizeString::get("client.settings.menuKey.desc"));
        set->value = &this->menuKey;
        set->callback = [this](Setting& set) {
            Latite::getScreenManager().get<HUDEditor>().key = this->getMenuKey();
        };
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("ejectKey", LocalizeString::get("client.settings.ejectKey.name"),
                                             LocalizeString::get("client.settings.ejectKey.desc"));
        set->value = &this->ejectKey;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("menuBlurEnabled",
                                             LocalizeString::get("client.settings.menuBlurEnabled.name"),
                                             LocalizeString::get("client.settings.menuBlurEnabled.desc"));
        set->value = &this->menuBlurEnabled;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("useDX11", LocalizeString::get("client.settings.useDX11.name"),
                                             LocalizeString::get("client.settings.useDX11.desc"));
        set->value = &this->useDX11;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("forceDisableVSync", L"Force Disable VSync",
                                             L"Forces VSync in fullscreen. May cause freezing, overheating, and screen tearing on some devices.\nRestart your game upon disabling this setting.");
        // Add in LocalizeString calls when this setting has been translated to other languages.
        /*
        auto set = std::make_shared<Setting>("forceDisableVSync", LocalizeString::get("client.settings.forceDisableVSync.name"),
            LocalizeString::get("client.settings.forceDisableVSync.desc"));
            */
        set->value = &this->forceDisableVSync;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("commandPrefix", LocalizeString::get("client.settings.commandPrefix.name"),
                                             LocalizeString::get("client.settings.commandPrefix.desc"));
        set->value = &this->commandPrefix;
        set->visible = false;
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("menuIntensity", LocalizeString::get("client.settings.menuIntensity.name"),
                                             LocalizeString::get("client.settings.menuIntensity.desc"));
        set->value = &this->menuBlur;
        set->min = FloatValue(1.f);
        set->max = FloatValue(30.f);
        set->interval = FloatValue(1.f);
        this->getSettings().addSetting(set);
    }
    {
        auto set = std::make_shared<Setting>("accentColor", LocalizeString::get("client.settings.accentColor.name"),
                                             LocalizeString::get("client.settings.accentColor.desc"));
        set->value = &this->accentColor;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("minViewBob", L"Minimal View Bob (UNSTABLE)", L"Only bob the item in hand, not the camera");
        set->value = &this->minimalViewBob;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("minecraftRenderer",
                                             LocalizeString::get("client.settings.minecraftRenderer.name"),
                                             LocalizeString::get("client.settings.minecraftRenderer.desc"));
        set->value = &this->minecraftRenderer;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("textShadow", LocalizeString::get("client.settings.textShadow.name"),
                                             LocalizeString::get("client.settings.textShadow.desc"));
        set->value = &this->textShadow;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("secondaryFont", LocalizeString::get("client.settings.secondaryFont.name"),
                                             LocalizeString::get("client.settings.secondaryFont.desc"));
        set->value = &this->secondaryFont;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("mcRendererFont",
                                             LocalizeString::get("client.settings.mcRendererFont.name"),
                                             LocalizeString::get("client.settings.mcRendererFont.desc"));
        set->enumData = &this->mcRendFont;
        set->value = set->enumData->getValue();
        set->enumData->addEntry({
            0, LocalizeString::get("client.settings.mcRendererFont.default.name"),
            LocalizeString::get("client.settings.mcRendererFont.default.desc")
        });
        set->enumData->addEntry({
            1, LocalizeString::get("client.settings.mcRendererFont.notoSans.name"),
            LocalizeString::get("client.settings.mcRendererFont.notoSans.desc")
        });
        this->getSettings().addSetting(set);
    }

    {
        //auto set = std::make_shared<Setting>("broadcastClientUsage", "Latite Client Presence", "If you leave this on, others with Latite will see that you are using Latite and you will see other people who use Latite.");
        //set->value = &this->broadcastUsage;
        //this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>(""
                                             "centerCursor", LocalizeString::get("client.settings.centerCursor.name"),
                                             LocalizeString::get("client.settings.centerCursor.desc"));
        set->value = &this->centerCursorMenus;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("snapLines", LocalizeString::get("client.settings.snapLines.name"),
                                             LocalizeString::get("client.settings.snapLines.desc"));
        set->value = &this->snapLines;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("detectLanguage",
                                             LocalizeString::get("client.settings.detectLanguage.name"),
                                             LocalizeString::get("client.settings.detectLanguage.desc"));
        set->value = &this->detectLanguageSetting;
        this->getSettings().addSetting(set);
    }

    {
        auto set = std::make_shared<Setting>("rgbSpeed", L"RGB Speed", L"How fast the RGB color cycles");
        set->value = &this->rgbSpeed;
        set->min = FloatValue(0.f);
        set->max = FloatValue(3.f);
        set->interval = FloatValue(0.1f);
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

void Latite::initL10n() {
    l10nData = LocalizeData();
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

void Latite::initLanguageSetting() {
    auto set = std::make_shared<Setting>("language", L"Language",
        L"The client's language.");
    set->enumData = &this->clientLanguage;
    set->value = set->enumData->getValue();
    set->userUpdateCallback = [](Setting&) {
        Latite::getNotifications().push(LocalizeString::get("client.message.languageSwitchHelper.name"));
        };

    for (int i = 0; auto & lang : l10nData->getLanguages()) {
        set->enumData->addEntry({
            i, util::StrToWStr(lang->name)
            });
        i++;
    }
    this->getSettings().addSetting(set);
}

void Latite::detectLanguage() {
    if (!this->getDetectLanguageSetting()) return;

    winrt::hstring topUserLanguage = winrt::Windows::System::UserProfile::GlobalizationPreferences::Languages().GetAt(0);
    winrt::Windows::Globalization::Language language{topUserLanguage};
    std::string systemLanguage = winrt::to_string(language.LanguageTag());

    // Get the language code by itself EXCEPT for portuguese and chinese variants
    // en-US -> en
    std::regex pattern(R"(^([a-z]{2})-[A-Z]{2}$)");
    std::smatch match;
    if (std::regex_match(systemLanguage, match, pattern) && systemLanguage.find("pt") == std::string::npos &&
        systemLanguage.find("zh") == std::string::npos) {
        systemLanguage = match[1].str();
    }

    Latite::getSettings().forEach([&](std::shared_ptr<Setting> set) {
        if (set->name() == "language") {
            for (int i = 0; i < l10nData->getLanguages().size(); ++i) {
                const std::shared_ptr<LocalizeData::Language>& lang = l10nData->getLanguages()[i];
                if (systemLanguage == lang->langCode) {
                    ValueType* value = set->enumData->getValue();
                    if (value) {
                        *value = EnumValue(i);
                        return;
                    }
                }
            }
        }
    });
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
        getModuleManager().forEach([](std::shared_ptr<Module> mod) {
            mod->setBlocked(false);
            });
    }

    if (std::get<BoolValue>(centerCursorMenus) && 
        SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed()) {
        RECT r = { 0, 0, 0, 0 };
        GetClientRect(SDK::GameCore::get()->hwnd, &r);
        SetCursorPos(r.left + r.right / 2, r.top + r.bottom / 2);
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
            Latite::getClientMessageQueue().display(
                util::WFormat(LocalizeString::get("client.settings.dx11EnabledMsg.name")));
        }
        else {
            Latite::getRenderer().setShouldReinit();
        }
        lastDX11 = std::get<BoolValue>(useDX11);
    }

    rgbHue += SDK::ClientInstance::get()->minecraft->timer->alpha * 0.005f * std::get<FloatValue>(rgbSpeed);
    if (rgbHue > 1.f) {
        rgbHue = 0.f;
    }
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
                default:
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
}

void Latite::onRenderOverlay(Event& evG) {
    auto& ev = reinterpret_cast<RenderOverlayEvent&>(evG);

    if (getRenderer().getFontFamily2() != std::get<TextValue>(secondaryFont).str) {
        getRenderer().updateSecondaryFont(std::get<TextValue>(secondaryFont).str);
    }

    while (!this->dxRenderQueue.empty()) {
        auto& latest = this->dxRenderQueue.front();
        latest(ev.getDeviceContext());
        this->dxRenderQueue.pop();
    }

    static auto time = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - time) > 5s) {
        Latite::writeServerIP();
    }
}

void Latite::onPacketReceive(Event& evG) {
    // disabled
    auto& ev = reinterpret_cast<PacketReceiveEvent&>(evG);
}

void Latite::onTick(Event& ev) {
    updateModuleBlocking();
}

void Latite::loadLanguageConfig(std::shared_ptr<Setting> languageSetting) {
    this->getSettings().forEach([&](std::shared_ptr<Setting> set) {
        if (set->name() == languageSetting->name()) {
            std::visit([&](auto&& obj) {
                *set->value = obj;
                set->update();
                }, languageSetting->resolvedValue);
        }
        });
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

void Latite::writeServerIP() {
    std::string server;
    std::filesystem::path serverIPTextPath = util::GetLatitePath() / XOR_STRING("serverip.txt");

    SDK::RakNetConnector* connector = SDK::RakNetConnector::get();
    if (connector && !connector->dns.empty()) {
        server = connector->dns;
    }
    else {
        server = "none";
    }

    static std::string lastServer = "";
    if (server != lastServer) {
        std::ofstream ofs(serverIPTextPath, std::ios::trunc);
        if (ofs.is_open()) {
            if (!ofs.fail()) {
                ofs << server;
                ofs.close();
            }
        }
    }
    lastServer = server;
}