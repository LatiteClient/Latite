#pragma once
#include "util/ChakraUtil.h"

class JsClass {
public:
	JsClass(class JsPlugin* owner, const wchar_t* name): owner(owner), name(name) {
	}
	JsClass(JsClass&) = delete;
	JsClass(JsClass&&) = delete;

	virtual ~JsClass() {

		// this crash, probably becuase the runtime doesnt exist
		//if (prototype != JS_INVALID_REFERENCE) JS::JsRelease(prototype, nullptr);
		//if (constructor != JS_INVALID_REFERENCE) JS::JsRelease(constructor, nullptr);
	}
	
	virtual void prepareFunctions() {};

	void createConstructor(JsNativeFunction callback, void* callbackState = nullptr) {
		this->constructor = callback;
		JS::JsCreateFunction(callback, callbackState, &constructor);
		JS::JsAddRef(constructor, nullptr);
	}

	JsValueRef errConstructCall() {
		Chakra::ThrowError(name + std::wstring(L" cannot be invoked without 'new'"));
		return JS_INVALID_REFERENCE;
	}

	JsValueRef errNoConstruct() {
		Chakra::ThrowError(name + std::wstring(L" cannot be constructed"));
		return JS_INVALID_REFERENCE;
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
	[[nodiscard]] JsValueRef getConstructor() { return constructor; }
	[[nodiscard]] const wchar_t* getName() { return name; }
protected:
	class JsPlugin* owner;
	const wchar_t* name;
	JsNativeFunction constructorCallback = nullptr;
	JsValueRef constructor = JS_INVALID_REFERENCE;
	JsValueRef prototype = JS_INVALID_REFERENCE;
public:
};