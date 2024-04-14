#pragma once
#include "JsModuleClass.h"
#include "client/feature/module/script/JsHudModule.h"

class JsHudModuleClass : public JsModuleClass {
public:
	static JsValueRef CALLBACK jsConstructor2(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsHudModuleClass*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		if (!Chakra::VerifyArgCount(argCount, 6)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsNumber}, {arguments[5], JsBoolean}})) return JS_INVALID_REFERENCE;

		auto jsm = new JsHUDModule(util::WStrToStr(Chakra::GetString(arguments[1])), util::WStrToStr(Chakra::GetString(arguments[2])), util::WStrToStr(Chakra::GetString(arguments[3])), Chakra::GetInt(arguments[4]), Chakra::GetBool(arguments[5]));
		return thi->construct(reinterpret_cast<JsModule*>(jsm), true);
	}

	inline static const wchar_t* class_name = L"HudModule";

	JsHudModuleClass(JsScript* owner, const wchar_t* name = class_name) : JsModuleClass(owner, name, false) {
		createConstructor(jsConstructor2, this);
	}

	void prepareFunctions() override {
		__super::prepareFunctions();
		Chakra::DefineFunc(prototype, hudModuleGetRect, XW("getRect"), this);
		Chakra::DefineFunc(prototype, hudModuleSetRect, XW("setRect"), this);
		Chakra::DefineFunc(prototype, hudModuleSetBounds, XW("setBounds"), this);
		Chakra::DefineFunc(prototype, hudModuleGetSize, XW("getSize"), this);
		Chakra::DefineFunc(prototype, hudModuleSetSize, XW("setSize"), this);
	};

private:
	static JsValueRef CALLBACK hudModuleGetRect(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK hudModuleSetRect(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK hudModuleSetBounds(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK hudModuleGetSize(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK hudModuleSetSize(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};