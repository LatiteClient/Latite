#pragma once
#include "../JsWrapperClass.h"
#include "util/ChakraUtil.h"
#include "client/screen/script/JsScreen.h"

class JsScreenClass : public JsWrapperClass<JsScreen> {
protected:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsScreenClass*>(callbackState);
		if (!Chakra::VerifyArgCount(argCount, 3)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsNumber} })) return JS_INVALID_REFERENCE;

		auto screen = new JsScreen(arguments[0], util::WStrToStr(Chakra::GetString(arguments[1])));
		screen->key = Chakra::GetInt(arguments[2]);

		JsValueRef obj = thi->construct(screen, false);
		return obj;
	}

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsScreenClass*>(callbackState);
		auto scn = Get(arguments[0]);
		std::string add = std::format("{} ({})", util::WStrToStr(thi->name), scn->getName());
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}

	static JsValueRef CALLBACK isActiveCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		return Get(arguments[0])->isActive() ? Chakra::GetTrue() : Chakra::GetFalse();
	}

	static JsValueRef CALLBACK getKeyCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		return Chakra::MakeInt(Get(arguments[0])->key);
	}

	static JsValueRef CALLBACK setKeyCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsNumber} })) return JS_INVALID_REFERENCE;

		Get(arguments[0])->key = Chakra::GetInt(arguments[1]);
		return JS_INVALID_REFERENCE;
	}

	static JsValueRef CALLBACK isHoverCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {

		if (!Chakra::VerifyArgCount(argCount, 2, true, true)) return JS_INVALID_REFERENCE;
		if (!Chakra::VerifyParameters({ {arguments[1], JsObject}, {Chakra::TryGet(arguments, argCount, 2), JsObject}})) return JS_INVALID_REFERENCE;

		auto obj = reinterpret_cast<JsScreenClass*>(callbackState);

		auto rect = JsRect::ToRect(arguments[1]);
		
		
		Vec2 point = SDK::ClientInstance::get()->cursorPos;
		if (auto val = Chakra::TryGet(arguments, argCount, 2)) {
			point = JsVec2::ToVec2(val);
		}

		return Get(arguments[0])->shouldSelect(rect, point) ? Chakra::GetTrue() : Chakra::GetFalse();
	}
public:
	inline static const wchar_t* class_name = L"Screen";

	JsScreenClass(class JsScript* owner) : JsWrapperClass(owner, class_name) {
		createConstructor(jsConstructor, this);
	}

	/// <summary>
	/// Constructs a JS screen.
	/// </summary>
	/// <param name="set">The C++ screen to create from.</param>
	/// <param name="finalize">Whether to destroy the C++ screen when the object goes out of scope or not.</param>
	/// <returns></returns>
	JsValueRef construct(JsScreen* screen, bool finalize) {
		auto obj = __super::construct(screen, finalize);

		Chakra::SetPropertyString(obj, L"name", util::StrToWStr(screen->getName()), true);
		return obj;
	}

	void prepareFunctions() override {
		Chakra::SetPropertyString(prototype, L"name", L"", true);
		Chakra::DefineFunc(prototype, isActiveCallback, L"isActive", this);
		Chakra::DefineFunc(prototype, getKeyCallback, L"getKey", this);
		Chakra::DefineFunc(prototype, setKeyCallback, L"setKey", this);
		Chakra::DefineFunc(prototype, isHoverCallback, L"isHover", this);
	};
};
