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

	JsValueRef dispatchEvent(std::wstring name, Event& ev)
	{
		for (auto& evs : this->eventListeners[name]) {
			JS::JsSetCurrentContext(evs.second);

			ev.arguments.push_back(evs.first);
			JsValueRef val;
			JS::JsCallFunction(evs.first, ev.arguments.data(), static_cast<unsigned short>(ev.arguments.size()), &val);
			return val;
		}
		return JS_INVALID_REFERENCE;
	}
};