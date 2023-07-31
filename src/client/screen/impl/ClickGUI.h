#pragma once
#include "../Screen.h"
#include "api/eventing/Event.h"
#include "client/render/asset/Asset.h"
#include "util/DxUtil.h"
#include <memory>
#include <array>

class ClickGUI : public Screen {
public:
	ClickGUI();

	void onRender(Event& ev);
	void onCleanup(Event& ev);
	void onInit(Event& ev);
	void onKey(Event& ev);
	void onClick(Event& ev);

	void drawSetting(class Setting* set, struct Vec2 const& pos, class DXContext& dc, float size = 150.f);
protected:
	void onEnable(bool ignoreAnims) override;
	void onDisable() override;
private:

	ComPtr<ID2D1Bitmap1> shadowBitmap;

	// TODO: move this somewhere more public
	Asset latiteLogo{L"logo.png"};

	enum Tab {
		MODULES = 0,
		SETTINGS,
	} tab = MODULES;

	enum ModTab {
		ALL = 0,
		GAME,
		HUD,
		SCRIPT
	} modTab = ALL;

	struct ModContainer {
		std::string name;
		std::string description;
		std::shared_ptr<class IModule> mod;
		bool shouldRender = true;
		bool isExtended = false;
		float arrowRot = 180.f;
		float lerpArrowRot = 0.f;
		float lerpToggle = 0.f;
		Color toggleColorOn = {};
		Color toggleColorOff = {};

		static bool compare(ModContainer const& a, ModContainer const& b) {
			return a.name < b.name;
		}
	};

	d2d::Rect rect = {};

	Setting* activeSetting = nullptr;
	int capturedKey = 0;

	ComPtr<ID2D1Effect> compositeEffect;
	ComPtr<ID2D1LinearGradientBrush> gradientBrush;
	ComPtr<ID2D1GradientStopCollection> gradientStopCollection;
};