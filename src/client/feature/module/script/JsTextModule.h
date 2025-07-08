#pragma once
#include "client/script/JsEvented.h"
#include "client/script/JsScript.h"
#include <client/feature/module/TextModule.h>

class JsTextModule : public TextModule, public JsEvented {
public:
	JsTextModule(std::string const& name, std::wstring const& displayName,
		std::wstring const& desc, int key);

	~JsTextModule() {
		// FIXME: check if no leak
		//JS::JsRelease(object, nullptr);
	}

	void onEnable() override;
	void onDisable() override;
	bool shouldHoldToToggle() override;
	std::wstringstream text(bool isPreview, bool isEditor) override;
	void preRender(bool mcRend, bool isPreview, bool isEditor) override;

	JsScript* script = nullptr;
private:
	std::string displayName;
	std::wstring cachedText = L"";
	bool cachedHoldToToggle = false;

	JsValueRef object = JS_INVALID_REFERENCE;
	JsContextRef ctx = JS_INVALID_REFERENCE;
};