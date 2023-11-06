#pragma once
#include "../JsClass.h"

class JsTextureClass final : public JsClass {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsTextureClass*>(callbackState);
		return thi->errNoConstruct();
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		return Chakra::MakeString(L"[object " + std::wstring(reinterpret_cast<JsTextureClass*>(callbackState)->name) +L"]");
	}

	static JsValueRef CALLBACK loadCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {



		return JS_INVALID_REFERENCE;
	}
public:
	inline static const wchar_t* class_name = L"Texture";

	JsTextureClass(class JsScript* owner) : JsClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(Vec2 const& vec) {
		JsValueRef obj;
		JS::JsCreateObject(&obj);
		JS::JsSetPrototype(obj, getPrototype());
		return obj;
	}

	void prepareFunctions() override {
		// Static functions
		Chakra::DefineFunc(constructor, loadCallback, L"load", this);

		// Member functions
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
	};
};