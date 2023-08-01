#include "GameScriptingObject.h"
#include "../JsScript.h"
#include "client/script/class/impl/JsVec2.h"
#include "sdk/common/client/game/ClientInstance.h"

void GameScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	this->createWorldObject();
	Chakra::DefineFunc(object, getMousePosCallback, L"getMousePos");
}

void GameScriptingObject::createWorldObject() {
}

JsValueRef GameScriptingObject::getMousePosCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsScript* script;
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JS::JsGetContextData(ctx, reinterpret_cast<void**>(&script));

	auto vec2 = script->findClass<JsVec2>(L"Vector2");
	if (vec2) {
		return vec2->construct(sdk::ClientInstance::get()->cursorPos);
	}

	// could not find Vector2
	__debugbreak();
	return Chakra::GetUndefined();
}
