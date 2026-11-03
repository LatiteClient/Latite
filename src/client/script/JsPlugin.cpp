#include "pch.h"
#include "JsPlugin.h"
#include <sstream>
#include "util/Chakrautil.h"
#include "util/Util.h"
#include "PluginManager.h"
#include "util/Logger.h"
#include "util/HttpClient.h"
#include <bcrypt.h>
#include <limits>

void JsPlugin::checkTrusted() {
#ifdef LATITE_DEBUG
    trusted = true;
    return;
#else

    trusted = false;
    auto response = http::request(
        L"GET", L"https://raw.githubusercontent.com/Imrglop/Latite-Releases/refs/heads/main/script_hashes.txt");
    if (!response) {
        Logger::Warn("Plugin trust check request failed: {}", response.error());
        return;
    }
    if (response->status != 200) {
        Logger::Warn("Plugin trust check request failed: HTTP {}", response->status);
        return;
    }

    std::string stringResponse(response->body.begin(), response->body.end());
    std::optional<std::wstring> currentHash = getHash(this->getPath());

    std::istringstream stream(stringResponse);
    std::string hash;
    std::getline(stream, hash);

    if (!hash.empty() && hash.back() == '\r') hash.pop_back();
    std::transform(hash.begin(), hash.end(), hash.begin(), [](unsigned char digit) {
        return static_cast<char>(std::tolower(digit));
    });

    if (currentHash == util::StrToWStr(hash)) {
        trusted = true;
        return;
    }
#endif
}

JsPlugin::JsPlugin(std::wstring const& relPath) {
    this->path = this->path / relPath;
    this->relFolderPath = relPath;

    this->name = getFolderName();
}

bool JsPlugin::load() {
    if (!std::filesystem::exists(path / MAIN_SCRIPT_NAME)) {
        return false;
    }

    if (!fetchPluginData()) {
        Logger::Warn("Could not load plugin metadata from the plugin metadata file.");
    }

    if (JS::JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &this->runtime) != JsNoError) return false;

    this->mainScript = loadAndRunScript(std::wstring(MAIN_SCRIPT_NAME.data(), MAIN_SCRIPT_NAME.size()));

    if (mainScript == nullptr) {
        JS::JsDisableRuntimeExecution(this->runtime);
        JS::JsDisposeRuntime(this->runtime);
        this->runtime = JS_INVALID_RUNTIME_HANDLE;
    }

    // Check plugin permissions
    checkTrusted();
    return mainScript != nullptr;
}

void JsPlugin::unload() {
    if (runtime != JS_INVALID_RUNTIME_HANDLE) {
        JS::JsSetCurrentContext(JS_INVALID_REFERENCE);
        JS::JsDisableRuntimeExecution(runtime);
        JS::JsCollectGarbage(runtime);
        JS::JsDisposeRuntime(runtime);
        runtime = JS_INVALID_RUNTIME_HANDLE;
    }
}

void JsPlugin::handleAsyncOperations() {
    for (auto& scr : this->scripts) {
        scr->handleAsyncOperations();
    }
}

bool JsPlugin::fetchPluginData() {
    std::ifstream ifs { this->getPath() / PLUGIN_MANIFEST_FILE };
    if (ifs.fail()) {
        return false;
    }

    json manifest;
    try {
        manifest = json::parse(ifs);
    } catch (json::parse_error& e) {
        Logger::Warn("Parse Error when loading plugin: {}", e.what());
        return false;
    }

    if (!manifest.is_object()) return false;

    auto& name = manifest["name"];
    auto& author = manifest["author"];
    auto& version = manifest["version"];
    auto& desc = manifest["description"];

    if (name.is_string()) this->name = util::StrToWStr(name.get<std::string>());
    if (author.is_string()) this->author = util::StrToWStr(author.get<std::string>());
    if (version.is_string()) this->version = util::StrToWStr(version.get<std::string>());
    if (desc.is_string()) this->description = util::StrToWStr(desc.get<std::string>());

    return true;
}

std::shared_ptr<JsScript> JsPlugin::loadAndRunScript(std::wstring relPath) {
    auto scr = std::make_shared<JsScript>(this, this->getPath() / relPath, relPath);
    scr->load();

    JsValueRef global;
    JS::JsGetGlobalObject(&global); // this doesn't add a reference that needs to be freed

    JsValueRef modObj;
    JS::JsCreateObject(&modObj);

    JsValueRef exportsObj;
    JS::JsCreateObject(&exportsObj);

    Chakra::SetProperty(global, L"module", modObj);
    Chakra::SetProperty(global, L"exports", exportsObj); // So the object.setproperty(exports, esmodule)... works

    Chakra::SetProperty(modObj, L"exports",
                        Chakra::GetProperty(modObj, L"exports")); // This should just reference module.exports, but idk

    auto err = scr->runScript();
    if (err != JsNoError) {
        Latite::getPluginManager().handleErrors(err);
        return nullptr;
    }

    this->scripts.push_back(scr);
    return scr;
}

std::shared_ptr<JsScript> JsPlugin::loadOrFindModule(JsScript* script, std::wstring name) {
    auto path = script->getFolderPath() / name;
    if (std::filesystem::exists(name)) path = name;

    for (auto& scr : this->scripts) {
        auto absolutepath = std::filesystem::absolute(scr->getPath());
        auto absolutepath2 = std::filesystem::absolute(path);
        if (absolutepath == absolutepath2) {
            Logger::Info("Already found module {}", util::WStrToStr(name));
            return scr;
        }
    }
    Logger::Info("Loading module {}", util::WStrToStr(name));

    auto scr = std::make_shared<JsScript>(this, path, name);
    scr->load();
    this->scripts.push_back(scr);

    JsValueRef global;
    JS::JsGetGlobalObject(&global); // this doesn't add a reference that needs to be freed

    JsValueRef modObj;
    JS::JsCreateObject(&modObj);

    JsValueRef exportsObj;
    JS::JsCreateObject(&exportsObj);

    Chakra::SetProperty(global, L"module", modObj);
    Chakra::SetProperty(global, L"exports", exportsObj); // So the object.setproperty(exports, esmodule)... works

    Chakra::SetProperty(modObj, L"exports",
                        Chakra::GetProperty(modObj, L"exports")); // This should just reference module.exports, but idk

    auto err = scr->runScript();

    if (err != JsNoError) {
        Latite::getPluginManager().handleErrors(err);
        return nullptr;
    }
    return scr;
}

std::wstring JsPlugin::getCertificate() {
    std::wifstream ifs(getPath() / "certificate");
    if (ifs.fail()) {
#if LATITE_DEBUG
        Logger::Info("Failed to get certificate");
#endif
        return L"";
    }

    std::wstringstream wss;
    wss << ifs.rdbuf();
    return wss.str();
}

std::optional<std::wstring> JsPlugin::getHash(std::filesystem::path const& main) {
    std::vector<std::filesystem::path> jsFiles;

    std::function<void(std::filesystem::path const&)> iterate = [&jsFiles,
                                                                 &iterate](std::filesystem::path const& path) -> void {
        for (auto& fil : std::filesystem::directory_iterator(path)) {
            if (fil.is_directory()) {
                iterate(fil);
                continue;
            }
            if (fil.path().string().ends_with(".js")) { // I think I should still be fine with unicode filepaths
                jsFiles.push_back(fil);
            }
        }
    };

    std::wstringstream toHash;

    bool hasRead = false;

    iterate(main);

    for (auto& fil : jsFiles) {
        std::wifstream ifs(fil);
        if (!ifs.fail()) {
            toHash << ifs.rdbuf();
            hasRead = true;
        }
#if LATITE_DEBUG
        else {
            Logger::Warn("[Script] Error opening script file {} to get hash: {}", fil.string(), errno);
        }
#endif
    }

    // iterate(std::filesystem::path(indexPath).parent_path());
    if (hasRead) {
        auto input = util::WStrToStr(toHash.str());
        if (input.size() > std::numeric_limits<ULONG>::max()) return std::nullopt;

        BCRYPT_ALG_HANDLE algorithm = nullptr;
        if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) {
            return std::nullopt;
        }

        std::array<unsigned char, 32> digest {};
        auto status = BCryptHash(algorithm, nullptr, 0, reinterpret_cast<unsigned char*>(input.data()),
                                 static_cast<ULONG>(input.size()), digest.data(), static_cast<ULONG>(digest.size()));
        BCryptCloseAlgorithmProvider(algorithm, 0);
        if (status < 0) return std::nullopt;

        constexpr wchar_t hex[] = L"0123456789abcdef";
        std::wstring result;
        result.reserve(digest.size() * 2);
        for (auto byte : digest) {
            result += hex[byte >> 4];
            result += hex[byte & 0x0f];
        }
        return result;
    }
    return std::nullopt;
}

bool JsPlugin::isTrusted() {
    return trusted;
}
