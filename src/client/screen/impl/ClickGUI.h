#pragma once
#include "../Screen.h"
#include "api/eventing/Event.h"
#include "client/render/asset/Asset.h"
#include "api/feature/setting/Setting.h"
#include "util/DxUtil.h"
#include "client/ui/TextBox.h"
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
	void onChar(Event& ev);

	bool drawSetting(class Setting* set, struct Vec2 const& pos, class DXContext& dc, float size = 150.f, float fTextWidth = 0.21f);
	
	bool shouldSelect(d2d::Rect rc, Vec2 const& pt) override;

	void drawColorPicker();
protected:
	void onEnable(bool ignoreAnims) override;
	void onDisable() override;
private:
	struct ColorPicker {
		Setting* setting = nullptr;
		StoredColor* selectedColor = nullptr;
		HSV pickerColor = {};
		float svModX = 1.f;
		float svModY = 0.f;
		float hueMod = 0.f;
		float opacityMod = 1.f;

		bool isEditingHue = false;
		bool isEditingOpacity = false;
		bool isEditingSV = false;

		bool queueClose = false;
		bool dragging = false;
		Vec2 dragOffs = {};
	} colorPicker{};

	ui::TextBox searchTextBox{};
	std::vector<ui::TextBox> pickerTextBoxes{};

	ComPtr<ID2D1Bitmap1> shadowBitmap;
	bool modClip = false;

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
	d2d::Rect cPickerRect = {};

	Setting* activeSetting = nullptr;
	int capturedKey = 0;
	float adaptedScale = 0.f;

	ComPtr<ID2D1Effect> compositeEffect;
};