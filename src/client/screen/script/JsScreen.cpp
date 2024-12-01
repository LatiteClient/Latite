#include "pch.h"
#include "JsScreen.h"
#include <client/event/impl/RendererInitEvent.h>
#include <client/event/impl/RenderGameEvent.h>
#include "sdk/common/client/gui/controls/VisualTree.h"
#include "sdk/common/client/gui/controls/UIControl.h"

JsScreen::JsScreen(JsValueRef object, std::string name) : name(std::move(name)) {
	JS::JsGetCurrentContext(&this->ctx);

	Chakra::GetStringProperty(object, L"name");
	key = Chakra::GetIntProperty(object, L"key");

	Eventing::get().listen<RenderLayerEvent>(this, (EventListenerFunc)&JsScreen::onRender, 1, true);
	Eventing::get().listen<KeyUpdateEvent>(this, (EventListenerFunc)&JsScreen::onKey, 1);
	Eventing::get().listen<ClickEvent>(this, (EventListenerFunc)&JsScreen::onClick, 1);

	this->eventListeners[L"enable"] = {};
	this->eventListeners[L"disable"] = {};
	this->eventListeners[L"render"] = {};
	this->eventListeners[L"key"] = {};
	this->eventListeners[L"mouse"] = {};
}

void JsScreen::onRender(::Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	auto view = ev.getScreenView();
	if (view->visualTree->rootControl->name == "debug_screen") {
		Chakra::SetContext(ctx);

		Event ev{ L"render", {} };
		auto ret = dispatchEvent(ev.name, ev);
		if (ret != JS_INVALID_REFERENCE) {
			Chakra::Release(ret);
		}
	}
}

void JsScreen::onKey(::Event& evG) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evG);
	Event sEv{ L"key", {Chakra::MakeInt(ev.getKey()), ev.isDown() ? Chakra::GetTrue() : Chakra::GetFalse() }};
	auto ret = dispatchEvent(sEv.name, sEv);
	if (ret != JS_INVALID_REFERENCE) {
		ev.setCancelled(Chakra::GetBool(ret));
		Chakra::Release(ret);
	}
}

void JsScreen::onClick(::Event& evG) {
	auto& ev = reinterpret_cast<ClickEvent&>(evG);
	Event sEv{ L"mouse", {Chakra::MakeInt(ev.getMouseButton()), Chakra::MakeInt(ev.getWheelDelta()), ev.isDown() ? Chakra::GetTrue() : Chakra::GetFalse() } };
	auto ret = dispatchEvent(sEv.name, sEv);
	if (ret != JS_INVALID_REFERENCE) {
		ev.setCancelled(Chakra::GetBool(ret));
		Chakra::Release(ret);
	}
}

void JsScreen::onEnable(bool ignoreAnims) {
	Latite::get().queueForClientThread([this, ignoreAnims]() {
		Chakra::SetContext(ctx);

		Event ev{ L"enable", { (ignoreAnims ? Chakra::GetTrue() : Chakra::GetFalse()) }};
		auto ret = dispatchEvent(ev.name, ev);
		if (ret != JS_INVALID_REFERENCE) {
			Chakra::Release(ret);
		}
		});
}

void JsScreen::onDisable() {
	Latite::get().queueForClientThread([this]() {
		Chakra::SetContext(ctx);
		Event ev{ L"disable", {  } };
		auto ret = dispatchEvent(ev.name, ev);
		if (ret != JS_INVALID_REFERENCE) {
			Chakra::Release(ret);
		}
		});
}
