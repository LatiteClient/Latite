#pragma once
#include "../JsClass.h"
#include "util/LMath.h"
#include "util/ChakraUtil.h"

class JsColor : public JsClass {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsColor*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		if (argCount == 1) {
			return thi->construct(d2d::Color(0.f, 0.f, 0.f, 0.f));
		}

		if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsNumber}, {arguments[2], JsNumber } })) return JS_INVALID_REFERENCE;

		return thi->construct(d2d::Color((float)Chakra::GetNumber(arguments[1]), (float)Chakra::GetNumber(arguments[2]),
			(float)Chakra::GetNumber(arguments[3]), (float)Chakra::GetNumber(arguments[4])));
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsColor*>(callbackState);
		auto col = ToColor(arguments[0]);
		std::string add = std::format("{} (r={:.2}, g={:.2} b={:.2} a={:.2})", util::WStrToStr(thi->name), col.r, col.g, col.b, col.a);
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	inline static const wchar_t* class_name = L"Color";

	JsColor(class JsScript* owner) : JsClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(d2d::Color const& rec) {
		JsValueRef obj;
		JS::JsCreateObject(&obj);
		JS::JsSetPrototype(obj, getPrototype());
		Chakra::SetPropertyNumber(obj, L"r", static_cast<double>(rec.r), true);
		Chakra::SetPropertyNumber(obj, L"g", static_cast<double>(rec.g), true);
		Chakra::SetPropertyNumber(obj, L"b", static_cast<double>(rec.b), true);
		Chakra::SetPropertyNumber(obj, L"a", static_cast<double>(rec.a), true);
		return obj;
	}

	void prepareFunctions() override {
		Chakra::SetPropertyNumber(prototype, L"r", 0.0, true);		Chakra::SetPropertyNumber(prototype, L"b", 0.0, true);
		Chakra::SetPropertyNumber(prototype, L"a", 1.0, true);

		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
	};

	static d2d::Color ToColor(JsValueRef obj) {
		return {
			static_cast<float>(Chakra::GetNumberProperty(obj, L"r")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"g")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"b")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"a")),
		};
	}
};