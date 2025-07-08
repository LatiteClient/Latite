#pragma once
#pragma once
#include "../ScriptingObject.h"

class OSScriptingObject : public ScriptingObject {
	static JsValueRef CALLBACK openUri(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK playSound(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
public:
	inline static int objectID = -1;
	OSScriptingObject(int id) : ScriptingObject(id, L"os") { OSScriptingObject::objectID = id; }

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

private:
};