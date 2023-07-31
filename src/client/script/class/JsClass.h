#pragma once
#include "util/ChakraUtil.h"

class JsClass {
public:
	JsClass(const wchar_t* name): name(name) {
	}
	JsClass(JsClass&) = delete;
	JsClass(JsClass&&) = delete;

	~JsClass() {
		if (prototype != JS_INVALID_REFERENCE) JS::JsRelease(prototype, nullptr);
	}

	void createConstructor(JsNativeFunction callback, void* callbackState = nullptr) {
		this->constructor = callback;
		JS::JsCreateFunction(callback, callbackState, &constructor);
		Chakra::DefineFunc(constructor, callback, name, callbackState);
	}

	void createPrototype() {
		JsValueRef global;

		JS::JsCreateObject(&prototype);
		JS::JsAddRef(prototype, nullptr);

		JS::JsGetGlobalObject(&global);
		JsPropertyIdRef propId;
		JS::JsGetPropertyIdFromName(name, &propId);

		JsValueRef classRef;
		JS::JsGetProperty(global, propId, &classRef);

		JS::JsGetPropertyIdFromName(L"prototype", &propId);
		JS::JsSetProperty(classRef, propId, prototype, true);

		JS::JsRelease(global, nullptr);
		JS::JsRelease(classRef, nullptr);
	}
	[[nodiscard]] JsValueRef getPrototype() { return prototype; }
protected:
	const wchar_t* name;
	JsNativeFunction constructorCallback = nullptr;
	JsValueRef constructor = JS_INVALID_REFERENCE;
	JsValueRef prototype = JS_INVALID_REFERENCE;
};