#include "pch.h"
// brace yourselves
#include "PluginManager.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageQueue.h"
#include "client/event/Eventing.h"
#include "client/event/events/UpdateEvent.h"

#include "mc/common/client/game/ClientInstance.h"
#include "mc/common/client/player/LocalPlayer.h"

#include "util/Logger.h"
#include <memory>

#include <expected>
#include "util/HttpClient.h"

#include "JsPlugin.h"

PluginManager::PluginManager() {
}

std::filesystem::path PluginManager::getPluginsDir() {
    return util::GetLatitePath() / "Plugins";
}

std::filesystem::path PluginManager::getPrerunPluginsDir() {
    return getPluginsDir();
}

std::shared_ptr<JsPlugin> PluginManager::loadPlugin(std::wstring const& folderPath, bool run) {
    auto& fPathW = folderPath;
    auto scriptPath = getPluginsDir() / fPathW;
    if (!std::filesystem::exists(scriptPath)) return nullptr;

    for (auto& scr : this->items | std::views::values) {
        if (std::filesystem::absolute(scr->getPath()) == std::filesystem::absolute(scriptPath)) {
            Latite::getClientMessageQueue().push(
                util::Format(std::format("Plugin {} is already loaded.", util::WStrToStr(scr->getName()))));
            return nullptr;
        }
    }

    auto myScript = std::make_shared<JsPlugin>(scriptPath);
    if (!myScript->load()) return nullptr;
    this->items.insert({ util::WStrToStr(myScript->getFolderName()), myScript });

    if (run) {
        Event::Value val { L"scriptName" };
        val.val = myScript->getName();
        Event::Value val2 { L"scriptVersion" };
        val2.val = myScript->getVersion();
        Event::Value val3 { L"scriptAuthor" };
        val3.val = myScript->getAuthor();
        Event newEv { L"load-script", { val, val2, val3 }, false };
        dispatchEvent(newEv);
    }
    return myScript;
}

std::shared_ptr<JsPlugin> PluginManager::getPluginByName(std::wstring const& name) {
    for (auto& script : items | std::views::values) {
        if (script->getName() == name) {
            return script;
        }
    }
    return nullptr;
}

bool PluginManager::isPluginInstalled(std::string const& id) {
    for (auto& dirEntry : std::filesystem::directory_iterator(getPluginsDir())) {
        if (dirEntry.is_directory()) {
            if (dirEntry.path().stem().string() == id) {
                return true;
            }
        }
    }
    return false;
}

std::shared_ptr<class JsPlugin> PluginManager::getPluginById(std::string const& id) {
    auto it = items.find(id);

    if (it == items.end()) return nullptr;

    return it->second;
}

void PluginManager::popScript(std::shared_ptr<JsPlugin> ptr) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->second == ptr) {
            unloadScript(it->second);
            items.erase(it);
            return;
        }
    }
}

void PluginManager::reportError(JsValueRef except, std::wstring filePath) {
    auto str = util::WStrToStr(Chakra::ToString(except));
    auto stack = Chakra::GetStringProperty(except, L"stack");

    std::stringstream ss;
    ss << "&c" << util::WStrToStr(stack);

    Latite::getClientMessageQueue().display(util::Format(ss.str()));
    Logger::Info("(plugin/{}) ({}) {}", util::WStrToStr(JsScript::getThis()->getPlugin()->getName()),
                 JsScript::getThis()->getRelativePath().string(), util::WStrToStr(stack));

    // not sure if you release the exception or not, will do it anyway
    Chakra::Release(except);
}

void PluginManager::handleErrors(JsErrorCode code) {
    JsContextRef ctx;
    JS::JsGetCurrentContext(&ctx);
    JsScript* script = JsScript::getThis();
    if (script) {
        if (code == JsErrorScriptException) {
            JsValueRef except;
            JS::JsGetAndClearException(&except);
            reportError(except, script->data.name);
        } else if (code != JsNoError) {
            Latite::getClientMessageQueue().display(
                util::Format(std::format("&cA JS error occurred in script {}: JsErrorCode 0x{:X}",
                                         util::WStrToStr(script->data.name), (int)code)));
            Logger::Info("(plugin/{}) ({}) Js ErrorCode: 0x{:X}", util::WStrToStr(script->getPlugin()->getName()),
                         script->getRelativePath().string(), (int)code);
        }
    }
}

bool PluginManager::loadPrerunScripts() {
    if (!scriptingSupported()) {
        Logger::Warn("Scripting is not supported. Please try restarting your game.");
        return false;
    }

    auto prerunPath = getPrerunPluginsDir();
    std::filesystem::create_directory(prerunPath);
    for (auto& dirEntry : std::filesystem::directory_iterator(prerunPath)) {
        if (dirEntry.is_directory()) {
            auto pluginJsonPath = dirEntry.path() / JsPlugin::PLUGIN_MANIFEST_FILE;
            std::ifstream ifs { pluginJsonPath };

            bool res = loadPlugin(dirEntry.path().filename().wstring(), true) == nullptr;

            if (!ifs.fail()) {
                try {
                    auto json = json::parse(ifs);
                    json["lastLaunch"] = res;
                    ifs.close();

                    std::ofstream ofs { pluginJsonPath };
                    if (!ofs.fail()) {
                        ofs << std::setw(4) << json;
                    }
                } catch (nlohmann::json::parse_error&) {}
            }
        }
    }
    return true;
}

void PluginManager::runScriptingOperations() {
    if (!scriptingSupported()) return;

    for (auto& plug : this->items | std::views::values) {
        plug->handleAsyncOperations();

        for (auto& scr : plug->getScripts()) {
            // timeouts
            for (auto it = scr->timeouts.begin(); it != scr->timeouts.end();) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - (*it)->createTime);
                if (duration.count() > (*it)->time) {
                    Chakra::SetContext((*it)->context);
                    JsValueRef res;
                    handleErrors(Chakra::CallFunction((*it)->callback, &(*it)->callback, 1, &res));
                    JS::JsRelease(res, nullptr);
                    scr->timeouts.erase(it);
                    continue;
                }
                ++it;
            }

            // intervals
            for (auto& tim : scr->intervals) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - tim->createTime);
                if (duration.count() > tim->time) {
                    Chakra::SetContext(tim->context);
                    JsValueRef res;
                    handleErrors(Chakra::CallFunction(tim->callback, &tim->callback, 1, &res));
                    JS::JsRelease(res, nullptr);
                    tim->createTime = now;
                }
            }
        }
    }

    // drawing
    Event ev { L"renderDX", {}, false };
    dispatchEvent(ev);
}

std::expected<void, std::string> PluginManager::installScript(std::string const& inName) {
    std::wstring registry = L"https://raw.githubusercontent.com/LatiteScripting/Scripts/master/Plugins";
    auto index = http::request(L"GET", registry + L"/plugins.json");
    if (!index) return std::unexpected(index.error());
    if (index->status != 200) {
        return std::unexpected("Could not fetch the plugin list (HTTP " + std::to_string(index->status) + ")");
    }

    nlohmann::json scriptsJson;
    try {
        scriptsJson = nlohmann::json::parse(index->body);
    } catch (nlohmann::json::exception const& error) {
        return std::unexpected("JSON parse error while installing plugin: " + std::string(error.what()));
    }

    if (!scriptsJson.is_object() || !scriptsJson.contains("plugins") ||
        !scriptsJson["plugins"].is_array()) {
        return std::unexpected("The plugin index is invalid");
    }

    auto& arr = scriptsJson["plugins"];
    for (auto& js : arr) {
        if (!js.is_object() || !js.contains("name") || !js["name"].is_string() ||
            !js.contains("files") || !js["files"].is_array()) {
            continue;
        }

        auto name = js["name"].get<std::string>();
        auto oName = js["name"].get<std::string>();
        auto woName = util::StrToWStr(oName);
        std::string in = inName;
        std::transform(in.begin(), in.end(), in.begin(), ::tolower);
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (in == name) {
            std::filesystem::path path = getPrerunPluginsDir() / woName;
            std::filesystem::create_directories(path);
            for (auto& fil : js["files"]) {
                if (!fil.is_string()) return std::unexpected("Invalid plugin file entry");

                auto fws = util::StrToWStr(fil.get<std::string>());
                auto download = http::request(L"GET", registry + L"/" + woName + L"/" + fws);
                if (!download) return std::unexpected(download.error());
                if (download->status != 200) {
                    return std::unexpected("Could not download plugin file (HTTP " +
                                           std::to_string(download->status) + ")");
                }

                auto downloadPath = path / fws;

                // make sure the directory we write the file to exists
                std::filesystem::create_directories(downloadPath.parent_path());

                std::ofstream ofs;
                ofs.open(path / fws, std::ios::binary);
                if (ofs.fail()) {
                    return std::unexpected("Error opening file: " + std::to_string(*_errno()));
                }
                ofs.write(reinterpret_cast<char const*>(download->body.data()), download->body.size());
                if (!ofs) return std::unexpected("Error writing plugin file");
                ofs.close();
            }
            return {};
        }
    }
    return std::unexpected("Could not find script " + inName);
}

std::vector<PluginManager::PluginInfo> PluginManager::fetchPluginsFromMarket() {
    std::vector<PluginInfo> list = {};

    std::wstring registry = L"https://raw.githubusercontent.com/LatiteScripting/Scripts/master/Plugins";
    auto index = http::request(L"GET", registry + L"/plugins.json");
    if (!index) {
        Latite::getClientMessageQueue().push(index.error());
        return list;
    }
    if (index->status != 200) return list;

    nlohmann::json scriptsJson;
    try {
        scriptsJson = nlohmann::json::parse(index->body);
    } catch (nlohmann::json::exception const&) {
        return list;
    }

    if (!scriptsJson.is_object() || !scriptsJson.contains("plugins") ||
        !scriptsJson["plugins"].is_array()) {
        Logger::Warn("The plugin index is invalid");
        return list;
    }

    auto& arr = scriptsJson["plugins"];

    for (auto& plug : arr) {
        if (!plug.is_object() || !plug.contains("name") || !plug["name"].is_string()) continue;

        std::string id = plug["name"].get<std::string>();
        auto pluginFolder = registry + L"/" + util::StrToWStr(id);

        auto manifest = http::request(L"GET", pluginFolder + L"/plugin.json");
        if (!manifest || manifest->status != 200) continue;

        json pluginJson;
        try {
            pluginJson = nlohmann::json::parse(manifest->body);
        } catch (nlohmann::json::exception const&) {
            continue;
        }

        if (!pluginJson.is_object()) continue;

        bool hasName = pluginJson.contains("name") && pluginJson["name"].is_string();
        bool hasAuthor = pluginJson.contains("author") && pluginJson["author"].is_string();
        bool hasVersion = pluginJson.contains("version") && pluginJson["version"].is_string();
        bool hasDescription = pluginJson.contains("description") && pluginJson["description"].is_string();

        if (hasName && hasAuthor && hasVersion && hasDescription) {
            list.emplace_back(PluginInfo { id, util::StrToWStr(pluginJson["name"].get<std::string>()),
                                           util::StrToWStr(pluginJson["author"].get<std::string>()),
                                           util::StrToWStr(pluginJson["version"].get<std::string>()),
                                           util::StrToWStr(pluginJson["description"].get<std::string>()) });
        } else {
            Logger::Info("Plugin '{}' has an invalid plugin.json", id);
        }
    }

    return list;
}

std::vector<PluginManager::PluginInfo> PluginManager::fetchPlugins() {
    std::vector<PluginInfo> list = {};
    return list;
}

void PluginManager::init() {
    auto scriptsPath = getPluginsDir();
    std::filesystem::create_directory(scriptsPath);

    initListeners();
    int id = 0;
    // this->objects.push_back(std::make_shared<ClientScriptingObject>(id++));
    // this->objects.push_back(std::make_shared<GameScriptingObject>(id++));
    // this->objects.push_back(std::make_shared<GraphicsScriptingObject>(id++));
    // this->objects.push_back(std::make_shared<Graphics3DScriptingObject>(id++));
}

void PluginManager::initListeners() {
    eventListeners[L"world-tick"] = {};
    eventListeners[L"join-game"] = {};
    eventListeners[L"leave-game"] = {};
    eventListeners[L"receive-chat"] = {};
    eventListeners[L"title"] = {};
    eventListeners[L"send-chat"] = {};
    eventListeners[L"render2d"] = {};
    eventListeners[L"renderDX"] = {};
    eventListeners[L"attack"] = {};
    eventListeners[L"key-press"] = {};
    eventListeners[L"click"] = {};
    eventListeners[L"render3d"] = {};
    eventListeners[L"app-suspended"] = {};
    eventListeners[L"load-script"] = {};
    eventListeners[L"unload-script"] = {};
    eventListeners[L"text-input"] = {};
    eventListeners[L"transfer"] = {};
    eventListeners[L"change-dimension"] = {};
    eventListeners[L"pre-move"] = {};
    eventListeners[L"post-move"] = {};
    eventListeners[L"set-score"] = {};
    eventListeners[L"modal-form-request"] = {};
}

void PluginManager::unloadScript(std::shared_ptr<JsPlugin> ptr) {
    Event::Value val { L"scriptName" };
    val.name = L"scriptName";
    val.val = ptr->getName();
    Event newEv { L"unload-script", { val }, false };
    dispatchEvent(newEv);

    for (auto& ev : this->eventListeners) {
        if (ev.second.size() > 0) {
            auto it = ev.second.begin();
            while (it != ev.second.end()) {
                auto& scr = ptr->getScripts();
                auto removeIt = std::remove_if(scr.begin(), scr.end(), [&](const auto& script) {
                    return !script || std::get<2>(*it) == script->getContext();
                });

                if (removeIt != scr.end()) {
                    Chakra::SetContext(std::get<2>(*it));
                    unsigned int refCount;
                    JS::JsRelease(std::get<1>(*it), &refCount);
                    ev.second.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}

void PluginManager::unloadAll() {
    std::vector<std::shared_ptr<JsPlugin>> scripts;
    for (auto& script : this->items | std::views::values) {
        scripts.push_back(script);
    }

    for (auto& script : scripts) {
        popScript(script);
    }
}

bool PluginManager::hasPermission(JsPlugin* script, Permission perm) {
    auto player = SDK::ClientInstance::get()->getLocalPlayer();
    if (!player) {
#if LATITE_DEBUG
        Logger::Warn("[Script] attempt to call hasPermission while player does not exist!");
#endif
        return false;
    }
    switch (perm) {
    case Permission::SendChat:
        return script->isTrusted() || player->getCommandPermissionLevel() > 0;
    case Permission::Operator:
        return player->getCommandPermissionLevel() > 0;
    }
    return false;
}

bool PluginManager::scriptingSupported() {
    return JS::JsAddRef;
}

void PluginManager::uninitialize() {
    for (auto& script : this->items | std::views::values) {
        popScript(script);
    }
}

bool PluginManager::dispatchEvent(Event& ev) {
    for (auto& lis : eventListeners) {
        if (lis.first == ev.type) {
            for (auto& l : lis.second) {
                Chakra::SetContext(std::get<2>(l));
                JsValueRef params[2] = {};
                // create the obj
                JS::JsGetUndefinedValue(params);
                JS::JsCreateObject(&params[1]);
                JS::JsAddRef(params[1], nullptr);

                if (ev.isCancellable) {
                    Event::Value val {};
                    val.val = false;
                    val.name = L"cancel";
                    ev.values.push_back(val);
                }

                for (auto& val : ev.values) {
                    JsPropertyIdRef idRef;

                    JsValueRef ref = JS_INVALID_REFERENCE;
                    switch (val.val.index()) {
                    case Event::Value::Bool: {
                        JS::JsBoolToBoolean(std::get<bool>(val.val), &ref);
                    } break;
                    case Event::Value::Number: {
                        JS::JsDoubleToNumber(std::get<double>(val.val), &ref);
                    } break;
                    case Event::Value::String: {
                        JS::JsPointerToString(std::get<std::wstring>(val.val).c_str(),
                                              std::get<std::wstring>(val.val).size(), &ref);
                    } break;
                    case Event::Value::EntityRef:

                    {
                        // TODO: Entity refs
                    } break;
                    default:
                        throw std::runtime_error("unknown value");
                        break;
                    }
                    JS::JsGetPropertyIdFromName(val.name.c_str(), &idRef);
                    JS::JsSetProperty(params[1], idRef, ref, true);
                    Chakra::Release(ref);
                }
                JsValueRef ret;

                // int refc1 = Chakra::GetRefCount(l.first);
                // int refc = Chakra::GetRefCount(params[1]);
                handleErrors(Chakra::CallFunction(std::get<1>(l), params, 2, &ret));

                if (ev.isCancellable) {
                    auto b = Chakra::GetBoolProperty(params[1], L"cancel");
                    if (b) return true;
                }

                Chakra::Release(ret);
                Chakra::Release(params[1]);
                // Chakra::Release(params[0]);
            }
            break;
        }
    }
    return false;
}
