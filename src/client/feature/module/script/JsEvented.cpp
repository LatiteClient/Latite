#include "JsEvented.h"
#include "client/Latite.h"
#include "client/script/ScriptManager.h"

JsValueRef JsEvented::dispatchEvent(std::wstring name, Event& ev) {
	for (auto& evs : this->eventListeners[name]) {
		JS::JsSetCurrentContext(evs.second);

		ev.arguments.push_back(evs.first);
		JsValueRef val;
		Latite::getScriptManager().handleErrors(JS::JsCallFunction(evs.first, ev.arguments.data(), static_cast<unsigned short>(ev.arguments.size()), &val));
		return val;
	}
	return JS_INVALID_REFERENCE;
}