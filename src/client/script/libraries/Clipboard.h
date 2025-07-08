#pragma once
#include "../JsLibrary.h"
#include <functional>
#include <thread>
#include "../JsPlugin.h"
#include <winrt/windows.storage.streams.h>

class Clipboard : public JsLibrary {
public:
	JsValueRef initialize(JsValueRef parent) override;
	Clipboard(JsScript* owner) : JsLibrary(owner, L"clipboard") {}
private:
	static JsValueRef CALLBACK get(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	static JsValueRef CALLBACK set(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
	
	static JsValueRef CALLBACK getBitmap(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState);
};