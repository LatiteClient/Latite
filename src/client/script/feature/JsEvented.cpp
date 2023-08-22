/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "JsEvented.h"
#include "client/Latite.h"
#include "client/script/ScriptManager.h"

JsValueRef JsEvented::dispatchEvent(std::wstring name, Event& ev) {
	for (auto& evs : this->eventListeners[name]) {
		JS::JsSetCurrentContext(evs.second);

		ev.arguments.insert(ev.arguments.begin(), evs.first);
		JsValueRef val;
		Latite::getScriptManager().handleErrors(JS::JsCallFunction(evs.first, ev.arguments.data(), static_cast<unsigned short>(ev.arguments.size()), &val));
		return val;
	}
	return JS_INVALID_REFERENCE;
}