#pragma once
#include "../Screen.h"
#include "client/render/asset/Asset.h"
#include "client/ui/TextBox.h"
#include <memory>
#include <array>
#include <map>
#include <optional>

class JsScreen : public Screen {
public:
	JsScreen(JsValueRef object);

	void onRender(class Event& ev);
	void onCleanup(Event& ev);
	void onInit(Event& ev);
	void onKey(Event& ev);
	void onClick(Event& ev);

	std::string getName() override { return name; }
protected:
	void onEnable(bool ignoreAnims) override;
	void onDisable() override;
private:
	JsValueRef object;
	JsValueRef renderFunc;
	std::string name;
};