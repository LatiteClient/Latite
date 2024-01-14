#include "pch.h"
#include "JsEvented.h"
#include "client/Latite.h"
#include "client/script/PluginManager.h"

JsValueRef JsEvented::dispatchEvent(std::wstring name, Event& ev) {
	for (auto& evs : this->eventListeners[name]) {
		JS::JsSetCurrentContext(evs.second);

		ev.arguments.insert(ev.arguments.begin(), evs.first);
		JsValueRef val;

		for (int argu = 0; auto & arg : ev.arguments) {
			if (Chakra::GetRefCount(arg) == 0) {
				Logger::Warn("refCount for argument {} is 0!", argu);
			}

			++argu;
		}

		Latite::getPluginManager().handleErrors(Chakra::CallFunction(evs.first, ev.arguments.data(), static_cast<unsigned short>(ev.arguments.size()), &val));
		return val;
	}
	return JS_INVALID_REFERENCE;
}