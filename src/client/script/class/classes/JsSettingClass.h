#pragma once
#include "../JsWrapperClass.h"
#include "util/ChakraUtil.h"
#include "client/feature/setting/script/JsSetting.h"

class JsSettingClass : public JsWrapperClass<Setting> {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsSettingClass*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		Chakra::ThrowError(thi->name + std::wstring(L" cannot be constructed"));
		return JS_INVALID_REFERENCE;
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsSettingClass*>(callbackState);
		auto set = Get(arguments[0]);
		std::string add = std::format("{} ({})", util::WStrToStr(thi->name), set->name());
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}

	static JsValueRef CALLBACK getValueCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK setValueCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK setCondition(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
public:
	inline static const wchar_t* class_name = L"Setting";

	JsSettingClass(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	/// <summary>
	/// Constructs a JS setting.
	/// </summary>
	/// <param name="set">The C++ setting to create from.</param>
	/// <param name="finalize">Whether to destroy the C++ setting when the object goes out of scope or not.</param>
	/// <returns></returns>
	JsValueRef construct(Setting* set, bool finalize) {
		auto obj = __super::construct(set, finalize);

		Chakra::SetPropertyString(obj, L"name", util::StrToWStr(set->name()), true);
		Chakra::SetPropertyString(obj, L"description", set->desc(), true);
		Chakra::SetPropertyString(obj, L"displayName", set->getDisplayName(), true);
		return obj;
	}

	void prepareFunctions() override {
		Chakra::SetPropertyString(prototype, L"name", L"", true);
		Chakra::SetPropertyString(prototype, L"description", L"", true);
		Chakra::SetPropertyString(prototype, L"displayName", L"", true);

		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
		Chakra::DefineFunc(prototype, getValueCallback, XW("getValue"), this);
		Chakra::DefineFunc(prototype, setValueCallback, XW("setValue"), this);
		Chakra::DefineFunc(prototype, setCondition, XW("setCondition"), this);
	};
};