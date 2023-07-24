#pragma once
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include "chakra/ChakraCore.h"

class JsScript final {
public:
	JsContextRef ctx;
	JsSourceContext sCtx = 1;
	JsRuntimeHandle runtime;
	std::string runError = "";

	std::wstring indexPath;
	std::wstring relFolderPath;
	std::wifstream indexStream;
	std::wstring loadedScript;

	std::vector<JsValueRef> ownedEvents;
	//std::vector<std::shared_ptr<JsLibrary>> libraries;

	struct ScriptDefinedData {
		std::wstring name;
		std::wstring version;
		std::wstring author;
		std::wstring description;
	} data;

	JsScript(std::wstring const& indexPath);
	bool load();
	
	void loadPrototypes();
	void loadScriptObjects();
	void fetchScriptData();
	void unload();
	void handleAsyncOperations();

	JsErrorCode runScript();

	~JsScript() { if (runtime != JS_INVALID_RUNTIME_HANDLE) unload(); }
};