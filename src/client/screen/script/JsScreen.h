#pragma once
#include "../Screen.h"
#include "client/render/asset/Asset.h"
#include "client/ui/TextBox.h"
#include <memory>
#include <array>
#include <map>
#include <optional>
#include "client/script/feature/JsEvented.h"


class JsScreen : public Screen, public JsEvented {
public:
	JsScreen(JsValueRef object, JsValueRef renderFunc);

	void onRender(class Event& ev);
	void onKey(Event& ev);
	void onClick(Event& ev);

	std::string getName() override { return name; }

	JsValueRef getObject() const { return object; }
protected:
	void onEnable(bool ignoreAnims) override;
	void onDisable() override;
private:
	JsValueRef object = JS_INVALID_REFERENCE;
	JsValueRef renderFunc = JS_INVALID_REFERENCE;
	JsContextRef ctx = JS_INVALID_REFERENCE;

	std::string name;
};