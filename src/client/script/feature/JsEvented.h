#pragma once
#include "util/ChakraUtil.h"
#include <map>

class JsEvented {
public:
	JsEvented() {
	}

	~JsEvented() {
		//for (auto& ev : this->eventListeners) {
		//	for (auto& lis : ev.second) {
		//		Chakra::SetContext(lis.second);
		//		JS::JsRelease(lis.first, nullptr);
		//	}
		//}
	}

	struct Event {
		std::wstring name;
		std::vector<JsValueRef> arguments;

		Event(const std::wstring& name, std::vector<JsValueRef> arguments)
			: name(name), arguments(arguments)
		{
		}
	};

	std::map<std::wstring_view, std::vector<std::pair<JsValueRef, JsContextRef>>> eventListeners = {};

	JsValueRef dispatchEvent(Event& ev);
};