#include "pch.h"
#include "JsModuleClass.h"
#include "JsSettingClass.h"
#include "../../JsPlugin.h"

#include "client/feature/module/script/JsHudModule.h"
#include <client/feature/module/script/JsTextModule.h>
#include "JsColor.h"

JsValueRef JsModuleClass::moduleIsEnabled(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsModule* mod = nullptr;
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

	JsModule* mod = nullptr;
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
	JsModule* mod = nullptr;
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

	JsModule* mod = nullptr;
	JsHUDModule* hMod = nullptr;
	JsTextModule* tMod = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return Chakra::GetUndefined();
	}
	if (mod->isHud()) hMod = reinterpret_cast<JsHUDModule*>(mod);
	if (mod->isTextual()) tMod = reinterpret_cast<JsTextModule*>(mod);
	
	auto str = Chakra::GetString(arguments[1]);

	// need this to prevent undefined behavior
	if (tMod) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		tMod->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return arguments[0];
	} else if (hMod) {
		JsContextRef ctx;
		JS::JsGetCurrentContext(&ctx);
		JS::JsAddRef(arguments[2], nullptr);
		hMod->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
		return arguments[0];
	}
	else
		if (mod->eventListeners.find(str) != mod->eventListeners.end()) {
			JsContextRef ctx;
			JS::JsGetCurrentContext(&ctx);
			JS::JsAddRef(arguments[2], nullptr);
			mod->eventListeners[str].push_back(std::make_pair(arguments[2], ctx));
			return arguments[0];
		}
	Chakra::ThrowError(L"Unknown event " + str);
	return Chakra::GetUndefined();
}

JsValueRef JsModuleClass::moduleGetSettings(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsModule* mod = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&mod));
	if (mod) {
		JsValueRef array;
		JS::JsCreateArray(static_cast<unsigned int>(mod->settings->size()), &array);

		auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

		int i = 0;
		mod->settings->forEach([&](std::shared_ptr<Setting> set) {
			JsValueRef index = Chakra::MakeInt(i);
			auto setClass = thi->owner->getClass<JsSettingClass>();
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
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsBoolean} })) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = Get(arguments[0]);

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), disp, desc);

	*set->value = BoolValue(Chakra::GetBool(arguments[4]));
	set->defaultValue = BoolValue(Chakra::GetBool(arguments[4]));;

	auto setClass = thi->owner->getClass<JsSettingClass>();
	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}

JsValueRef JsModuleClass::moduleAddNumberSetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 8)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString},
		/*min*/{arguments[4], JsNumber},
		/*max*/{arguments[5], JsNumber},
		/*int*/{arguments[6], JsNumber},
		/*def*/{arguments[7], JsNumber} })) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = Get(arguments[0]);

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

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), disp, desc);

	*set->value = FloatValue(static_cast<float>(Chakra::GetNumber(arguments[7])));
	set->min = FloatValue(min);
	set->max = FloatValue(max);
	set->interval = FloatValue(intr);
	set->defaultValue = *set->value;

	auto setClass = thi->owner->getClass<JsSettingClass>();
	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}

JsValueRef JsModuleClass::moduleAddKeySetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsNumber} })) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = Get(arguments[0]);

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), disp, desc);

	*set->value = KeyValue(Chakra::GetInt(arguments[4]));
	set->defaultValue = *set->value;

	auto setClass = thi->owner->getClass<JsSettingClass>();
	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}

JsValueRef JsModuleClass::moduleAddTextSetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsString} })) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = Get(arguments[0]);

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), disp, desc);

	*set->value = TextValue(Chakra::GetString(arguments[4]));
	set->defaultValue = *set->value;

	auto setClass = thi->owner->getClass<JsSettingClass>();
	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}

JsValueRef JsModuleClass::moduleAddColorSetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsObject} })) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);

	auto mod = Get(arguments[0]);

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);
	auto col = JsColor::ToColor(arguments[4]);

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), disp, desc);

	*set->value = ColorValue(col.r, col.g, col.b, col.a);
	set->defaultValue = *set->value;

	auto setClass = thi->owner->getClass<JsSettingClass>();
	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}

JsValueRef JsModuleClass::moduleAddEnumSetting(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, { arguments[2], JsString}, {arguments[3], JsString}, {arguments[4], JsObject} })) return JS_INVALID_REFERENCE;

	JsValueRef options = arguments[4];
	JsValueRef values = Chakra::GetProperty(options, L"values");
	int defaultValue = Chakra::GetIntProperty(options, L"default_value");

	if (!values) {
		Chakra::ThrowError(L"Invalid enum data");
		return JS_INVALID_REFERENCE;
	}

	auto name = Chakra::GetString(arguments[1]);
	auto disp = Chakra::GetString(arguments[2]);
	auto desc = Chakra::GetString(arguments[3]);

	auto set = std::make_shared<JsSetting>(util::WStrToStr(name), disp, desc);

	*set->value = EnumValue(defaultValue);

	set->scriptEnumData = EnumData{};
	set->enumData = &*set->scriptEnumData;

	for (int i = 0; i < Chakra::GetIntProperty(values, L"length"); i++) {
		JsValueRef obj = JS_INVALID_REFERENCE;
		JS::JsGetIndexedProperty(values, Chakra::MakeInt(i), &obj);
		auto entryName = Chakra::GetStringProperty(obj, L"name");
		auto entryDesc = Chakra::GetStringProperty(obj, L"desc");
		if (obj != JS_INVALID_REFERENCE) {
			set->enumData->addEntry(EnumEntry{ i, entryName, entryDesc });
		}
	}

	auto thi = reinterpret_cast<JsModuleClass*>(callbackState);
	auto mod = Get(arguments[0]);

	auto setClass = thi->owner->getClass<JsSettingClass>();
	mod->settings->addSetting(set);

	return setClass->construct(set.get(), false /*do not destroy the setting once it goes out of scope, as module manager will handle that*/);
}