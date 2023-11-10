#pragma once
#include "../JsClass.h"
#include "util/LMath.h"
#include "util/ChakraUtil.h"

class JsRect : public JsClass {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsRect*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		if (argCount == 1) {
			return thi->construct(d2d::Rect(0.f, 0.f, 0.f, 0.f));
		}

		if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsNumber}, {arguments[2], JsNumber } })) return JS_INVALID_REFERENCE;

		return thi->construct(d2d::Rect((float)Chakra::GetNumber(arguments[1]), (float)Chakra::GetNumber(arguments[2]),
			(float)Chakra::GetNumber(arguments[3]), (float)Chakra::GetNumber(arguments[4])));
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsRect*>(callbackState);
		auto rect = ToRect(arguments[0]);
		std::string add = std::format("{} (left={:.2}, top={:.2} right={:.2} bottom={:.2})", util::WStrToStr(thi->name), rect.left, rect.top, rect.right, rect.bottom);
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	inline static const wchar_t* class_name = L"Rect";

	JsRect(class JsScript* owner) : JsClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(d2d::Rect const& rec) {
		JsValueRef obj;
		JS::JsCreateObject(&obj);
		JS::JsSetPrototype(obj, getPrototype());
		Chakra::SetPropertyNumber(obj, L"left", static_cast<double>(rec.left), true);
		Chakra::SetPropertyNumber(obj, L"top", static_cast<double>(rec.top), true);
		Chakra::SetPropertyNumber(obj, L"right", static_cast<double>(rec.right), true);
		Chakra::SetPropertyNumber(obj, L"bottom", static_cast<double>(rec.bottom), true);
		return obj;
	}

	void prepareFunctions() override {
		Chakra::SetPropertyNumber(prototype, L"left", 0.0, true);
		Chakra::SetPropertyNumber(prototype, L"top", 0.0, true);
		Chakra::SetPropertyNumber(prototype, L"right", 0.0, true);
		Chakra::SetPropertyNumber(prototype, L"bottom", 0.0, true);

		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);

	};

	static d2d::Rect ToRect(JsValueRef obj) {
		return {
			static_cast<float>(Chakra::GetNumberProperty(obj, L"left")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"top")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"right")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"bottom")),
		};
	}
};