#include "pch.h"
#include "JsHudModuleClass.h"
#include "client/feature/module/script/JsHudModule.h"
#include "JsRect.h"
#include "JsVec2.h"

JsValueRef JsHudModuleClass::hudModuleGetRect(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsHUDModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}

	return JsPlugin::getThis()->getClass<JsRect>()->construct(mod->getRectNonScaled());
}

JsValueRef JsHudModuleClass::hudModuleSetRect(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetUndefined();
	auto sz = Chakra::VerifyParameters({ {arguments[1], JsObject} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return Chakra::GetUndefined();
	}

	JsHUDModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}

	mod->setRect(d2d::Rect(static_cast<float>(Chakra::GetNumberProperty(arguments[1], L"left")),
		static_cast<float>(Chakra::GetNumberProperty(arguments[1], L"top")),
		static_cast<float>(Chakra::GetNumberProperty(arguments[1], L"right")),
		static_cast<float>(Chakra::GetNumberProperty(arguments[1], L"bottom"))));
	return Chakra::GetUndefined();
}

JsValueRef JsHudModuleClass::hudModuleSetBounds(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();
	auto sz = Chakra::VerifyParameters({ {arguments[1], JsNumber}, {arguments[2], JsNumber} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return Chakra::GetUndefined();
	}

	JsHUDModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}

	auto rc = mod->getRectNonScaled();
	rc.right = rc.left + static_cast<float>(Chakra::GetNumber(arguments[1]));
	rc.bottom = rc.top + static_cast<float>(Chakra::GetNumber(arguments[2]));

	mod->setRect(rc);
	return Chakra::GetUndefined();
}

JsValueRef JsHudModuleClass::hudModuleGetSize(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsHUDModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}

	return Chakra::MakeDouble(static_cast<double>(mod->getScale()));
}

JsValueRef JsHudModuleClass::hudModuleSetSize(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetUndefined();
	auto sz = Chakra::VerifyParameters({ {arguments[1], JsNumber} });
	JsHUDModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}

	mod->setScale(static_cast<float>(Chakra::GetNumber(arguments[1])));
	return Chakra::GetUndefined();
}