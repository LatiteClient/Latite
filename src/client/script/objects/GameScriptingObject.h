#pragma once
#include "../ScriptingObject.h"

class GameScriptingObject : public ScriptingObject {
	static JsValueRef CALLBACK emptyCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK sendChatCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK executeCommand(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK playSoundUI(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getLocalPlayerCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getConnectedServerCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getConnectedFeaturedServerCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getMousePosCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK getWorldCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK isLocalWorldCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK isInUICallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getScreenCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
public:
	inline static int objectID = -1;
	GameScriptingObject(int id) : ScriptingObject(id, L"game") { GameScriptingObject::objectID = id; }

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

private:
	void createWorldObject();

	JsValueRef worldObj = JS_INVALID_REFERENCE;
	static JsValueRef CALLBACK worldGetName(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK worldGetPlayers(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
public:
	~GameScriptingObject() override {
		JS::JsRelease(worldObj, nullptr);
	}
};