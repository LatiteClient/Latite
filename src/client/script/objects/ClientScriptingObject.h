#pragma once
#include "../ScriptingObject.h"

class ClientScriptingObject final : public ScriptingObject {

	static JsValueRef CALLBACK registerEventCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK runCommandCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK showNotifCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getMmgrCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);


public:
	inline static int objectID = -1;
	ClientScriptingObject(int id) : ScriptingObject(id, L"client") { objectID = id; }

	void initModuleManager();
	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

	~ClientScriptingObject() {
		JS::JsRelease(this->object, nullptr);
		JS::JsRelease(this->moduleManager, nullptr);
	}

	JsValueRef moduleManager = JS_INVALID_REFERENCE;
private:
	static JsValueRef CALLBACK mmgrRegisterModuleCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK mmgrDeregisterModuleCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK mmgrGetModuleByName(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK mmgrGetModuleById(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK mmgrForEachModule(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};