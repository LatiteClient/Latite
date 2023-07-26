#pragma once
#include "../ScriptingObject.h"

class ClientScriptingObject final : public ScriptingObject {

	static JsValueRef CALLBACK registerEventCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

public:
	inline static int objectID = -1;
	ClientScriptingObject(int id) : ScriptingObject(id) { objectID = id; }

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

	~ClientScriptingObject() {
		JS::JsRelease(this->object, nullptr);
	}
};