#pragma once
#include "JsClass.h"

template <typename T>
class JsWrapperClass : public JsClass {
protected:
	static JsValueRef CALLBACK jsNoConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsWrapperClass*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		Chakra::ThrowError(thi->name + std::wstring(L" cannot be constructed"));
		return JS_INVALID_REFERENCE;
	}

	static JsValueRef CALLBACK defaultToString(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsWrapperClass*>(callbackState);
		std::string add = std::format("{}", util::WStrToStr(thi->name));
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	JsWrapperClass(class JsScript* owner, const wchar_t* className) : JsClass(owner, className) {}

	virtual JsValueRef construct(T* ptr, bool del) {
		JsValueRef obj;
		if (del) {
			JS::JsCreateExternalObject(ptr, [](void* obj) {
				delete reinterpret_cast<T*>(obj);
				}, &obj);
		}
		else {
			JS::JsCreateExternalObject(ptr, [](void*) {
				}, &obj);
		}
		JS::JsSetPrototype(obj, getPrototype());
		return obj;
	}

	static T* Get(JsValueRef obj) {
		T* ret = nullptr;
		JS::JsGetExternalData(obj, reinterpret_cast<void**>(&ret));
		return ret;
	}
};