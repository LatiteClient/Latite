#pragma once
#include "../JsWrapperClass.h"
#include "../../interop/classes/JsTexture.h"

class JsTextureClass final : public JsWrapperClass<JsTexture> {
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

		if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return JS_INVALID_REFERENCE;
		
		std::wstring name = Chakra::GetString(arguments[1]);
		auto thi = reinterpret_cast<JsTextureClass*>(callbackState);

		JS::JsAddRef(arguments[0], nullptr);
		JsTexture* tex = new JsTexture(name, false);
		return thi->construct(tex);
	}

	static JsValueRef CALLBACK getCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return JS_INVALID_REFERENCE;

		std::wstring name = Chakra::GetString(arguments[1]);
		auto thi = reinterpret_cast<JsTextureClass*>(callbackState);

		JS::JsAddRef(arguments[0], nullptr);
		JsTexture* tex = new JsTexture(name, true);
		return thi->construct(tex);
	}

	static JsValueRef CALLBACK reloadCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		auto thi = reinterpret_cast<JsTextureClass*>(callbackState);

		auto tex = Get(arguments[0]);
		if (tex) tex->reloadMinecraft();

		return Chakra::GetUndefined();
	}

	static JsValueRef CALLBACK disposeCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		JS::JsRelease(arguments[0], nullptr);
		return Chakra::GetUndefined();
	}
public:
	inline static const wchar_t* class_name = L"Texture";

	JsTextureClass(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(JsTexture* tex, bool finalize = true) {
		return __super::construct(tex, finalize);
	}

	void prepareFunctions() override {
		// Static functions
		Chakra::DefineFunc(constructor, loadCallback, L"load", this);
		Chakra::DefineFunc(constructor, getCallback, L"get", this);

		// Member functions
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
		Chakra::DefineFunc(prototype, reloadCallback, L"reload", this);
		Chakra::DefineFunc(prototype, disposeCallback, L"dispose", this);
	};
};