#pragma once
#include "client/script/JsEvented.h"
#include "client/feature/module/HUDModule.h"
#include "util/ChakraUtil.h"

class JsHUDModule : public HUDModule, public JsEvented {
public:
	JsHUDModule(std::string const& name, std::wstring const& displayName,
		std::wstring const& desc, int key, bool resizable);

	~JsHUDModule() {
		// FIXME: check if no leak
		//JS::JsRelease(object, nullptr);
	}

	void onEnable() override;
	void onDisable() override;
	bool shouldHoldToToggle() override;
	void render(DrawUtil& dc, bool, bool) override;
	void preRender(bool mcRend, bool isPreview, bool isEditor) override;

	class JsScript* script = nullptr;
private:
	std::string displayName;
	bool cachedHoldToToggle = false;

	JsValueRef object = JS_INVALID_REFERENCE;
	JsContextRef ctx = JS_INVALID_REFERENCE;
};