#include "JsModuleClass.h"

JsValueRef JsModuleClass::moduleIsEnabled(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 1)) return Chakra::GetFalse();

	JsModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (mod) {
		return mod->isEnabled() ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(L"Object is not a module");
	return Chakra::GetFalse();
}

JsValueRef JsModuleClass::moduleSetEnabled(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetUndefined();

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsBoolean } });
	if (!sz) {
		Chakra::ThrowError(sz.str);
		return Chakra::GetUndefined();
	}

	JsModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (mod) {
		mod->setEnabled(Chakra::GetBool(arguments[1]));
		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(L"Object is not a module");
	return Chakra::GetUndefined();
}

JsValueRef JsModuleClass::moduleIsBlocked(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 1)) return Chakra::GetFalse();

	JsModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (mod) {
		return mod->isBlocked() ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(L"Object is not a module");
	return Chakra::GetFalse();
}

JsValueRef JsModuleClass::moduleSetOnEvent(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsString }, {arguments[2], JsFunction} });
	if (!sz) {
		Chakra::ThrowError(sz.str);
		return Chakra::GetUndefined();
	}

	JsModule* mod;
	//JsHudModule* hMod = nullptr;
	//JsTextModule* tMod = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	//if (mod->isVisual()) hMod = reinterpret_cast<JsHudModule*>(mod);
	//if (mod->isTextual()) tMod = reinterpret_cast<JsTextModule*>(mod);
	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}
	auto str = Chakra::GetString(arguments[1]);

	// need this to prevent undefined behavior
#if 0
	if (tMod) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		tMod->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return Chakra::GetUndefined();
	}
	if (hMod) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		hMod->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return Chakra::GetUndefined();
	}
	else
#endif
	if (mod->eventListeners.find(str) != mod->eventListeners.end()) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		mod->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(L"Unknown event " + str);
	return Chakra::GetUndefined();
}