#pragma once
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include "chakra/ChakraCore.h"
#include "JsLibrary.h"
#include "ScriptingObject.h"
#include "class/JsClass.h"
#include "JsScript.h"

class JsPlugin final {
public:
	JsSourceContext sCtx = 1;
	[[nodiscard]] bool isTrusted();
	[[nodiscard]] JsRuntimeHandle getRuntime() { return runtime; }
	[[nodiscard]] std::filesystem::path getMainIndexPath() { return mainScript->getPath(); }
	[[nodiscard]] std::filesystem::path getPath() { return path; }
	[[nodiscard]] std::wstring getRelFolderPath() { return relFolderPath; }
	[[nodiscard]] std::wstring getFolderName() { return path.filename(); }

	[[nodiscard]] std::wstring getName() { return name; }
	[[nodiscard]] std::wstring getAuthor() { return author; }
	[[nodiscard]] std::wstring getVersion() { return version; }
	[[nodiscard]] std::wstring getDescription() { return description; }

	inline static constexpr std::wstring_view MAIN_SCRIPT_NAME = L"main.js";
	inline static constexpr std::wstring_view PLUGIN_MANIFEST_FILE = L"plugin.json";
	
	JsPlugin(std::wstring const& relativePath);

	bool load();
	void unload();
	void handleAsyncOperations();
	bool fetchPluginData();

	std::shared_ptr<JsScript> loadAndRunScript(std::wstring relPath);
	std::shared_ptr<JsScript> loadOrFindModule(JsScript* script, std::wstring name);

	[[nodiscard]] std::wstring getCertificate();
	[[nodiscard]] static std::optional<std::wstring> getHash(std::filesystem::path const& main);

	std::vector<std::shared_ptr<JsScript>>& getScripts() { return scripts; }

	~JsPlugin() { if (runtime != JS_INVALID_RUNTIME_HANDLE) unload(); }

	enum class UserPermission {
		SYSTEM_ACCESS
	};

	bool hasPermission(UserPermission permission) {
		if (isTrusted())
			return true;

		for (auto perm : userPermissions) {
			if (perm == permission) {
				return true;
			}
		}

		return false;
	}

	void grantPermission(UserPermission permission) {
		if (!hasPermission(permission))
			userPermissions.push_back(permission);
	}
private:
	bool trusted = false;
	void checkTrusted();

	std::vector<UserPermission> userPermissions;
	std::vector<std::shared_ptr<JsScript>> scripts;
	std::shared_ptr<JsScript> mainScript;
	std::wstring name;
	std::wstring author;
	std::wstring description;
	std::wstring version;

	JsRuntimeHandle runtime = JS_INVALID_RUNTIME_HANDLE;

	std::filesystem::path path;
	std::wstring relFolderPath;
};