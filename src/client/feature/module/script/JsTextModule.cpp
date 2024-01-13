#include "pch.h"
#include "JsTextModule.h"
#include <client/script/objects/D2DScriptingObject.h>

JsTextModule::JsTextModule(std::string const& name,
	std::string const& displayName, std::string const& desc, int key)
	: TextModule(name, displayName, desc, SCRIPT, 400.f, key) {

	script = JsScript::getThis();

	this->eventListeners[L"enable"] = {};
	this->eventListeners[L"disable"] = {};
	this->eventListeners[L"get-hold-to-toggle"] = {};
	this->eventListeners[L"render"] = {};
}



void JsTextModule::onEnable() {
	JS::JsSetCurrentContext(ctx);
	Event ev{ L"enable", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		Chakra::Release(ret);
	}
}

void JsTextModule::onDisable() {
	JS::JsSetCurrentContext(ctx);
	Event ev{ L"disable", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		Chakra::Release(ret);
	}
}

bool JsTextModule::shouldHoldToToggle() {
	JS::JsSetCurrentContext(ctx);
	Event ev{ L"get-hold-to-toggle", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		bool b;
		if (JS::JsBooleanToBool(ret, &b) == JsNoError) {
			return b;
		}
		else {
			Chakra::ThrowError(L"get-hold-to-toggle must return boolean");
		}
		Chakra::Release(ret);
	}
	return false;
}

std::wstringstream JsTextModule::text(bool isPreview, bool isEditor) {
	JS::JsSetCurrentContext(ctx);

	return std::wstringstream() << cachedText;
}

void JsTextModule::preRender(bool mcRend, bool isPreview, bool isEditor) {
	JS::JsSetCurrentContext(ctx);

	{
		Event ev{ L"text", { isPreview ? Chakra::GetTrue() : Chakra::GetFalse(), isEditor ? Chakra::GetTrue() : Chakra::GetFalse()} };
		auto ret = dispatchEvent(ev.name, ev);
		if (ret != JS_INVALID_REFERENCE) {
			const wchar_t* b;
			size_t len;
			if (JS::JsStringToPointer(ret, &b, &len) == JsNoError) {
				cachedText = b;
			}
			else {
				Chakra::ThrowError(L"getText must return string");
			}
			Chakra::Release(ret);
		}
	}

	{
		auto obj = script->getObject<D2DScriptingObject>();
		bool oMinecraftRend = obj->usingMinecraftRend();
		obj->setUseMinecraftRend(mcRend);
		Event ev{ L"render", { isPreview ? Chakra::GetTrue() : Chakra::GetFalse(), isEditor ? Chakra::GetTrue() : Chakra::GetFalse()} };
		dispatchEvent(ev.name, ev);
		obj->setUseMinecraftRend(oMinecraftRend);
	}
}
