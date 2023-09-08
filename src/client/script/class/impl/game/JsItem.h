#pragma once
#include "../../JsClass.h"
#include "sdk/common/world/Item.h"
#include "sdk/common/world/Item.h"

class JsItem : public JsClass {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsItem*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		Chakra::ThrowError(thi->name + std::wstring(L" cannot be constructed"));
		return JS_INVALID_REFERENCE;
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsItem*>(callbackState);
		auto item = ToItem(arguments[0]);
		std::string add = std::format("{} ({})", util::WStrToStr(thi->name), item->namespacedId);
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	JsItem(class JsScript* owner) : JsClass(owner, L"Item") {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(SDK::Item* item, bool del) {
		JsValueRef obj;
		if (del) {
			JS::JsCreateExternalObject(item, [](void* obj) {
				delete obj;
				}, &obj);
		}
		else {
			JS::JsCreateExternalObject(item, [](void*) {
				}, &obj);
		}
		JS::JsSetPrototype(obj, getPrototype());
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
	};

	static SDK::Item* ToItem(JsValueRef obj) {
		SDK::Item* it = nullptr;
		JS::JsGetExternalData(obj, reinterpret_cast<void**>(&it));
		return it;
	}
};