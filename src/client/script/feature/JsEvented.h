/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "util/ChakraUtil.h"
#include <map>

class JsEvented {
public:
	JsEvented() {
	}

	~JsEvented() {
		for (auto& ev : this->eventListeners) {
			for (auto& lis : ev.second) {
				JS::JsSetCurrentContext(lis.second);
				JS::JsRelease(lis.first, nullptr);
			}
		}
	}

	struct Event {
		std::wstring name;
		std::vector<JsValueRef> arguments;

		Event(const std::wstring& name, std::vector<JsValueRef> arguments)
			: name(name), arguments(arguments)
		{
		}
	};

	std::map<std::wstring, std::vector<std::pair<JsValueRef, JsContextRef>>> eventListeners = {};

	JsValueRef dispatchEvent(std::wstring name, Event& ev);
};