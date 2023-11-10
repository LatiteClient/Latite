#pragma once
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include "chakra/ChakraCore.h"
#include "lib/JsLibrary.h"
#include "ScriptingObject.h"
#include "class/JsClass.h"
#include "JsScript.h"

class JsPlugin final {
public:
	JsSourceContext sCtx = 1;
	[[nodiscard]] bool isTrusted() { return trusted; }
	[[nodiscard]] JsRuntimeHandle getRuntime() { return runtime; }
	[[nodiscard]] std::filesystem::path getMainIndexPath() { return mainScript->getPath(); }
	[[nodiscard]] std::filesystem::path getPath() { return path; }
	[[nodiscard]] std::wstring getRelFolderPath() { return relFolderPath; }
	[[nodiscard]] std::wstring getFolderName() { return path.filename(); }

	inline static constexpr std::wstring_view MAIN_SCRIPT_NAME = L"main.js";
	
	JsPlugin(std::wstring const& relativePath);

	bool load();
	void unload();
	void handleAsyncOperations();
	bool fetchPluginData();

	std::shared_ptr<JsScript> loadAndRunScript(std::wstring relPath);
	std::shared_ptr<JsScript> loadOrFindModule(std::wstring name);

	[[nodiscard]] std::wstring getCertificate();
	[[nodiscard]] static std::optional<std::wstring> getHash(std::filesystem::path const& main);

	std::vector<std::shared_ptr<JsScript>>& getScripts() { return scripts; }

	~JsPlugin() { if (runtime != JS_INVALID_RUNTIME_HANDLE) unload(); }
private:
	bool trusted = false;
	void checkTrusted();

	std::vector<std::shared_ptr<JsScript>> scripts;
	std::shared_ptr<JsScript> mainScript;
	std::wstring name;
	std::wstring author;
	std::wstring description;
	std::wstring version;

	JsRuntimeHandle runtime;

	std::filesystem::path path;
	std::wstring relFolderPath;
};