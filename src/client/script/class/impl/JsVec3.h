/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../JsClass.h"
#include "util/LMath.h"
#include "util/ChakraUtil.h"

class JsVec3 : public JsClass {
private:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		auto thi = reinterpret_cast<JsVec3*>(callbackState);
		if (!isConstructor) return thi->errConstructCall();

		if (argCount == 1) {
			return thi->construct(Vec3(0.f, 0.f, 0.f));
		}

		if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsNumber}, {arguments[2], JsNumber } })) return JS_INVALID_REFERENCE;

		return thi->construct(Vec3((float)Chakra::GetNumber(arguments[1]), (float)Chakra::GetNumber(arguments[2]), (float)Chakra::GetNumber(arguments[3])));
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsVec3*>(callbackState);
		auto vec3 = ToVec3(arguments[0]);
		std::string add = std::format("{} ({:.2}, {:.2} {:.2})", util::WStrToStr(thi->name), vec3.x, vec3.y, vec3.z);
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}
public:
	JsVec3(class JsScript* owner) : JsClass(owner, L"Vector3") {
		createConstructor(jsConstructor, this);
	}

	JsValueRef construct(Vec3 const& vec) {
		JsValueRef obj;
		JS::JsCreateObject(&obj);
		JS::JsSetPrototype(obj, getPrototype());
		Chakra::SetPropertyNumber(obj, L"x", static_cast<double>(vec.x), true);
		Chakra::SetPropertyNumber(obj, L"y", static_cast<double>(vec.y), true);
		Chakra::SetPropertyNumber(obj, L"z", static_cast<double>(vec.z), true);
		return obj;
	}

	void prepareFunctions() override {
		auto proto = getPrototype();
		Chakra::SetPropertyNumber(proto, L"x", 0.0, true);
		Chakra::SetPropertyNumber(proto, L"y", 0.0, true);
		Chakra::SetPropertyNumber(proto, L"z", 0.0, true);

		Chakra::DefineFunc(proto, toStringCallback, L"toString", this);
	};

	static Vec3 ToVec3(JsValueRef obj) {
		return {
			static_cast<float>(Chakra::GetNumberProperty(obj, L"x")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"y")),
			static_cast<float>(Chakra::GetNumberProperty(obj, L"z"))
		};
	}
};