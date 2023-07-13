#pragma once
#include <iostream>
#include <fstream>
#include <functional>
#include <string>

class JsScript final {
public:
	//JsContextRef ctx;
	//JsSourceContext sCtx = 1;
	//JsRuntimeHandle runtime;
	std::string runError = "";

	std::wstring indexPath;
	std::wstring relFolderPath;
	std::wifstream indexStream;
	std::wstring loadedScript;

	//std::vector<JsValueRef> ownedEvents;
	//std::vector<std::shared_ptr<JsLibrary>> libraries;

	struct ScriptDefinedData {
		std::wstring name;
		std::wstring version;
		std::wstring author;
		std::wstring description;
	} data;
};