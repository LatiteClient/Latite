#include "pch.h"
#include "JsEvented.h"
#include "client/Latite.h"
#include "client/script/PluginManager.h"

JsValueRef JsEvented::dispatchEvent(std::wstring name, Event& ev) {
	for (auto& evs : this->eventListeners[name]) {
		Chakra::SetContext(evs.second);

		ev.arguments.insert(ev.arguments.begin(), evs.first);
		JsValueRef val;

		Latite::getPluginManager().handleErrors(Chakra::CallFunction(evs.first, ev.arguments.data(), static_cast<unsigned short>(ev.arguments.size()), &val));
		return val;
	}
	return JS_INVALID_REFERENCE;
}