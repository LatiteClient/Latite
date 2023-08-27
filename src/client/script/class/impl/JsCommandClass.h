#pragma once
#include "../JsClass.h"
#include "client/feature/command/script/JsCommand.h"

class JsCommandClass : public JsClass {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsCommandClass*>(callbackState);
		auto cmd = ToCommand(arguments[0]);
		if (!cmd) {
			Chakra::ThrowError(L"Invalid command");
			return JS_INVALID_REFERENCE;
		}
		std::string add = std::format("{} ({})", util::WStrToStr(thi->name), cmd->name());
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}

	static JsValueRef CALLBACK setOnEvent(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
public:
	inline static const wchar_t* class_name = L"Command";

	JsCommandClass(class JsScript* owner) : JsClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(JsCommand* mod, bool finalize) {
		JsValueRef obj;
		if (finalize) {
			JS::JsCreateExternalObject(mod, [](void* obj) {
				delete obj;
				}, &obj);
		}
		else {
			JS::JsCreateExternalObject(mod, [](void*) {
				}, &obj);
		}

		JS::JsSetPrototype(obj, getPrototype());

		Chakra::SetPropertyString(obj, L"name", util::StrToWStr(mod->name()));
		Chakra::SetPropertyString(obj, L"description", util::StrToWStr(mod->desc()));
		// aliases
		JsValueRef array;
		JS::JsCreateArray(0, &array);
		Chakra::SetProperty(obj, L"aliases", array);

		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
		Chakra::DefineFunc(prototype, setOnEvent, L"on", this);
	};

	static JsCommand* ToCommand(JsValueRef obj) {
		JsCommand* mod = nullptr;
		JS::JsGetExternalData(obj, reinterpret_cast<void**>(&mod));
		return mod;
	}
};