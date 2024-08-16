#pragma once
#include "JsTextModuleClass.h"
#include "JsHUDModuleClass.h"
#include "client/feature/module/script/JsTextModule.h"

class JsTextModuleClass : public JsHudModuleClass {
public:
	static JsValueRef CALLBACK jsConstructor2(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsTextModuleClass*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsNumber} })) return JS_INVALID_REFERENCE;

		auto jsm = new JsTextModule(util::WStrToStr(Chakra::GetString(arguments[1])), util::WStrToStr(Chakra::GetString(arguments[2])), util::WStrToStr(Chakra::GetString(arguments[3])), Chakra::GetInt(arguments[4]));
		return thi->construct(reinterpret_cast<JsModule*>(jsm), true);
	}

	inline static const wchar_t* class_name = L"TextModule";

	JsTextModuleClass(JsScript* owner, const wchar_t* name = class_name) : JsHudModuleClass(owner, name) {
		createConstructor(jsConstructor2, this);
	}

	void prepareFunctions() override {
		__super::prepareFunctions();
		Chakra::DefineFunc(prototype, getLastTextCallback, XW("getLastText"), this);
	};

private:
	static JsValueRef CALLBACK getLastTextCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};