/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "JsModuleClass.h"
#include "JsSettingClass.h"
#include "../../JsScript.h"
#include "util/Logger.h"

JsValueRef JsModuleClass::moduleIsEnabled(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
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
	if (!Chakra::VerifyParameters({ {arguments[1], JsBoolean } })) return JS_INVALID_REFERENCE;

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

	if (!Chakra::VerifyParameters({ {arguments[1], JsString }, {arguments[2], JsFunction} })) return JS_INVALID_REFERENCE;

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

JsValueRef JsModuleClass::moduleGetSettings(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsModule* mod;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (mod) {
		JsValueRef array;
		JS::JsCreateArray(static_cast<unsigned int>(mod->settings->size()), &array);

		auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

		int i = 0;
		mod->settings->forEach([&](std::shared_ptr<Setting> set) {
			JsValueRef index = Chakra::MakeInt(i);
			auto setClass = thi->owner->findClass<JsSettingClass>(L"Setting");
			if (!setClass) {
				Logger::Fatal("Could not find setting class!!!");
				throw std::runtime_error("could not find setting class");
			}
			JS::JsSetIndexedProperty(array, index, setClass->construct(set.get(), false));
			Chakra::Release(index);
			++i;
			});
		
		return array;
	}
	Chakra::ThrowError(L"Object is not a module");
	return JS_INVALID_REFERENCE;
}

JsValueRef JsModuleClass::moduleAddBoolSetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}})) return JS_INVALID_REFERENCE;
	
	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = ToModule(arguments[0]);

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), util::WStrToStr(disp), util::WStrToStr(desc));

	*set->value = BoolValue(false);

	auto setClass = thi->owner->findClass<JsSettingClass>(L"Setting");

	if (!setClass) {
		Logger::Fatal("Could not find setting class!!!");
		throw std::runtime_error("could not find setting class");
	}

	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}

JsValueRef JsModuleClass::moduleAddNumberSetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 7)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}, 
		/*min*/{arguments[4], JsNumber},
		/*max*/{arguments[5], JsNumber},
		/*int*/{arguments[6], JsNumber}})) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = ToModule(arguments[0]);

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);

	float min = static_cast<float>(Chakra::GetNumber(arguments[4]));
	float max = static_cast<float>(Chakra::GetNumber(arguments[5]));
	float intr = static_cast<float>(Chakra::GetNumber(arguments[6]));

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), util::WStrToStr(disp), util::WStrToStr(desc));

	*set->value = FloatValue(0.f);
	set->min = FloatValue(min);
	set->max = FloatValue(max);
	set->interval = FloatValue(intr);

	auto setClass = thi->owner->findClass<JsSettingClass>(L"Setting");

	if (!setClass) {
		Logger::Fatal("Could not find setting class!!!");
		throw std::runtime_error("could not find setting class");
	}

	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}
