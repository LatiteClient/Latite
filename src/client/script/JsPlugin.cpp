#include "pch.h"
#include "JsPlugin.h"
#include <sstream>
#include "util/Chakrautil.h"
#include "util/Util.h"
#include "resource.h"

#include "PluginManager.h"

#include "Lib/Libraries/Filesystem.h"
#include "Lib/Libraries/Network.h"
#include "Lib/Libraries/Clipboard.h"

#include "objects/ClientScriptingObject.h"

#include "util/XorString.h"
#include "util/Logger.h"

#include "ScriptCertificate.h"

using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;


void JsPlugin::checkTrusted() {
	//if (this->loadedScript._Starts_with(util::StrToWStr(XOR_STRING("\"notrust\"")))) {
	//	trusted = false;
	//	return;
	//}

	auto hash = JsPlugin::getHash(getPath());

	if (hash) {
#if LATITE_DEBUG
		Logger::Info("{} {}", util::WStrToStr(hash.value()), util::WStrToStr(getCertificate()));
#endif
		if (this->getCertificate() == hash) {
			trusted = true;
			return;
		}
	}
	trusted = false;
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
		Logger::Warn("Could not load plugin metadata from the plugin.json");
	}

	if (JS::JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &this->runtime) != JsNoError) return false;

	this->mainScript = loadAndRunScript(std::wstring(MAIN_SCRIPT_NAME.data(), MAIN_SCRIPT_NAME.size()));
	
	if (mainScript == nullptr) {
		JS::JsDisableRuntimeExecution(this->runtime);
		JS::JsDisposeRuntime(this->runtime);
		this->runtime = JS_INVALID_RUNTIME_HANDLE;
	}
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
	std::ifstream ifs{this->getPath() / "plugin.json"};
	if (ifs.fail()) {
		return false;
	}

	json manifest;
	try {
		manifest = json::parse(ifs);
	}
	catch (json::parse_error& e) {
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

	Chakra::SetProperty(modObj, L"exports", Chakra::GetProperty(modObj, L"exports")); // This should just reference module.exports, but idk

	auto err = scr->runScript();
	if (err != JsNoError) {
		if (err == JsErrorScriptException) {
			JsValueRef except;
			JS::JsGetAndClearException(&except);
			Latite::getPluginManager().reportError(except, scr->getPlugin()->getFolderName() + L"/" + scr->getPath().filename().wstring());
		}
		return nullptr;
	}

	this->scripts.push_back(scr);
	return scr;
}

std::shared_ptr<JsScript> JsPlugin::loadOrFindModule(std::wstring name) {
	auto path = JsScript::getThis()->getFolderPath() / name;
	if (std::filesystem::exists(name)) path = name;

	for (auto& scr : this->scripts) {
		if (scr->getPath() == path) {
			Logger::Info("Already found module {}", util::WStrToStr(name));
			return scr;
		}
	}
	Logger::Info("Loading module {}", util::WStrToStr(name));

	auto scr = std::make_shared<JsScript>(this, path, name);
	scr->load();
	JsValueRef global;
	JS::JsGetGlobalObject(&global); // this doesn't add a reference that needs to be freed

	JsValueRef modObj;
	JS::JsCreateObject(&modObj);

	JsValueRef exportsObj;
	JS::JsCreateObject(&exportsObj);

	Chakra::SetProperty(global, L"module", modObj);
	Chakra::SetProperty(global, L"exports", exportsObj); // So the object.setproperty(exports, esmodule)... works

	Chakra::SetProperty(modObj, L"exports", Chakra::GetProperty(modObj, L"exports")); // This should just reference module.exports, but idk

	auto err = scr->runScript();

	if (err != JsNoError) {
		if (err == JsErrorScriptException) {
			JsValueRef except;
			JS::JsGetAndClearException(&except);
			Latite::getPluginManager().reportError(except, scr->getPlugin()->getFolderName() + L"/" + scr->getPath().filename().wstring());
		}
		return nullptr;
	}
	this->scripts.push_back(scr);
	return scr;
}

std::wstring JsPlugin::getCertificate() {
	std::wifstream ifs(getPath() / XOR_STRING("certificate"));
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
	using winrt::Windows::Security::Cryptography::Core::HashAlgorithmProvider;
	using winrt::Windows::Security::Cryptography::CryptographicBuffer;
	using winrt::Windows::Security::Cryptography::BinaryStringEncoding;

	std::vector<std::filesystem::path> jsFiles;

	std::function<void(std::filesystem::path const&)> iterate = [&jsFiles, &iterate](std::filesystem::path const& path) -> void {
		for (auto& fil : std::filesystem::directory_iterator(path)) {
			if (fil.is_directory()) {
				iterate(fil);
				continue;
			}
			if (fil.path().string().ends_with(XOR_STRING(".js"))) { // I think I should still be fine with unicode filepaths
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
			toHash << XOR_STRING(LATITE_SCRIPT_CERT_SALT);
			toHash << ifs.rdbuf();
			hasRead = true;
		}
#if LATITE_DEBUG
		else {
			Logger::Warn("[Script] Error opening script file {} to get hash: {}", fil.string(), errno);
		}
#endif
	}

	//iterate(std::filesystem::path(indexPath).parent_path());
	if (hasRead) {
		auto input = CryptographicBuffer::ConvertStringToBinary(toHash.str(), BinaryStringEncoding::Utf8);
		auto hasher = HashAlgorithmProvider::OpenAlgorithm(util::StrToWStr(XOR_STRING("SHA256")));
		auto hashed = hasher.HashData(input);

		auto htostr = CryptographicBuffer::EncodeToHexString(hashed);

		return htostr.c_str();
	}
	return std::nullopt;
}
