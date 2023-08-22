/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "JsModule.h"

JsModule::JsModule(std::string const& name, std::string const& displayName, std::string const& desc, int key)
	: Module(name, displayName, desc, SCRIPT, key, false)
{
	JS::JsAddRef(object, nullptr);
	JS::JsGetCurrentContext(&this->ctx);

	// TODO(script): isHoldToToggle
	this->eventListeners[L"enable"] = {};
	this->eventListeners[L"disable"] = {};
	this->eventListeners[L"get-hold-to-toggle"] = {};
}

void JsModule::onEnable()
{
	JS::JsSetCurrentContext(ctx);
	Event ev{ L"enable", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		Chakra::Release(ret);
	}
}

void JsModule::onDisable() {
	JS::JsSetCurrentContext(ctx);
	Event ev{ L"disable", {  } };
	auto ret = dispatchEvent(ev.name, ev);
	if (ret != JS_INVALID_REFERENCE) {
		Chakra::Release(ret);
	}
}

bool JsModule::shouldHoldToToggle() {
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
