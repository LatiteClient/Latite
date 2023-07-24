#include "JsScript.h"
#include <sstream>
#include "pch.h"
#include "util/ChakraUtil.h"

JsScript::JsScript(std::wstring const& indexPath) : indexPath(indexPath), ctx(JS_INVALID_REFERENCE), indexStream(), loadedScript(), runtime(JS_INVALID_RUNTIME_HANDLE){
}

bool JsScript::load() {
	try {
		indexStream.open(indexPath);
	}
	catch (std::exception&) {
		throw std::runtime_error("could not open index path");
		return false;
	}

	std::wstringstream buffer;
	buffer << indexStream.rdbuf();
	this->loadedScript = L"\"use strict\";" + buffer.str();
	this->indexStream.close();

	//this->libraries.push_back(std::make_shared<Filesystem>(this));
	//this->libraries.push_back(std::make_shared<Network>(this));

	if (JS::JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime)) return false;
	return JS::JsCreateContext(runtime, &this->ctx) == JsNoError;
}

void JsScript::fetchScriptData() {
	JS::JsSetCurrentContext(ctx);
	JsValueRef globalObj;
	JS::JsGetGlobalObject(&globalObj);

	JsPropertyIdRef scriptObjId;
	JS::JsGetPropertyIdFromName(L"script", &scriptObjId);

	JsValueRef script;
	JS::JsGetProperty(globalObj, scriptObjId, &script);

	this->data.name = Chakra::GetStringProperty(script, L"name");
	this->data.author = Chakra::GetStringProperty(script, L"author");
	this->data.version = Chakra::GetStringProperty(script, L"version");

	Chakra::Release(script);
	Chakra::Release(globalObj);
}
