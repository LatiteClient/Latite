#pragma once
#include "../../JsWrapperClass.h"
#include "sdk/common/world/Item.h"

class JsItem : public JsWrapperClass<SDK::Item> {
protected:

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsItem*>(callbackState);
		auto item = Get(arguments[0]);
		std::string add = std::format("{} ({})", util::WStrToStr(thi->name), item->namespacedId.getString());
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	inline static const wchar_t* class_name = L"Item";
	JsItem(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsNoConstructor, this);
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
		Chakra::SetPropertyString(obj, L"name", util::StrToWStr(item->namespacedId.getString()));
		Chakra::SetPropertyString(obj, L"translateName", util::StrToWStr(item->translateName));
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
	};
};