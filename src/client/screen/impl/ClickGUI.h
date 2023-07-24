#pragma once
#include "../Screen.h"
#include "api/eventing/Event.h"
#include "client/render/asset/Asset.h"
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
private:
	std::array<bool, 3> mouseButtons = {};
	std::array<bool, 3> lastMouseButtons = {};
	std::array<bool, 3> justClicked = {};

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

		static bool compare(ModContainer const& a, ModContainer const& b) {
			return a.name < b.name;
		}
	};

	ComPtr<ID2D1Bitmap1> blurBuffer;
	ComPtr<ID2D1Effect> compositeEffect;
};