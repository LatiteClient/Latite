#pragma once
#pragma once
#include "../ScriptingObject.h"

class OptionsScriptingObject : public ScriptingObject {
	static JsValueRef CALLBACK emptyCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getPlayerViewPerspective(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK setPlayerViewPerspective(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
public:
	inline static int objectID = -1;
	OptionsScriptingObject(int id) : ScriptingObject(id, L"options") { OptionsScriptingObject::objectID = id; }

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

private:
};