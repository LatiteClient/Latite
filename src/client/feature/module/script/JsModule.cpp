#include "pch.h"
#include "JsModule.h"

JsModule::JsModule(std::string const& name, std::wstring const& displayName, std::wstring const& desc, int key)
	: Module(name, displayName, desc, SCRIPT, key, false) {
	JS::JsGetCurrentContext(&this->ctx);

	this->eventListeners[L"enable"] = {};
	this->eventListeners[L"disable"] = {};
	this->eventListeners[L"get-hold-to-toggle"] = {};
}


void JsModule::onEnable() {

	if (!Latite::isMainThread()) {
		// hey lets hope that the js module doesnt disappear by the time this code executes
		Latite::get().queueForClientThread([this]() {
			Chakra::SetContext(ctx);
			Event ev{ L"enable", {  } };
			auto ret = dispatchEvent(ev.name, ev);
			if (ret != JS_INVALID_REFERENCE) {
				Chakra::Release(ret);
			}
			});
		return;
	}
	Chakra::SetContext(ctx);
	Event ev{ L"enable", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		Chakra::Release(ret);
	}
}

void JsModule::onDisable() {
	if (!Latite::isMainThread()) {
		// hey lets hope that the js module doesnt disappear by the time this code executes
		Latite::get().queueForClientThread([this]() {
			Chakra::SetContext(ctx);
			Event ev{ L"disable", {  } };
			auto ret = dispatchEvent(ev.name, ev);
			if (ret != JS_INVALID_REFERENCE) {
				Chakra::Release(ret);
			}
			});
		return;
	}

	Chakra::SetContext(ctx);
	Event ev{ L"disable", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		Chakra::Release(ret);
	}
}

bool JsModule::shouldHoldToToggle() {

	if (!Latite::isMainThread()) {
		return cacheHoldToToggle;
	}

	Chakra::SetContext(ctx);
	Event ev{ L"get-hold-to-toggle", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		bool b;
		if (JS::JsBooleanToBool(ret, &b) == JsNoError) {
			cacheHoldToToggle = b;
			return b;
		}
		else {
			Chakra::ThrowError(L"get-hold-to-toggle must return boolean");
		}
		Chakra::Release(ret);
	}
	cacheHoldToToggle = false;
	return false;
}
