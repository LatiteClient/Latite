#pragma once
#include "../Screen.h"
#include "client/render/asset/Asset.h"
#include "client/ui/TextBox.h"
#include <memory>
#include <array>
#include <map>
#include <optional>

class ClickGUI : public Screen {
public:
	ClickGUI();

	void onRender(class Event& ev);
	void onCleanup(Event& ev);
	void onInit(Event& ev);
	void onKey(Event& ev);
	void onClick(Event& ev);

	std::string getName() override { return "ClickGUI"; }
	float drawSetting(class Setting* set, class SettingGroup* group, struct Vec2 const& pos, class D2DUtil& dc, float size = 150.f, float fTextWidth = 0.21f, bool bypassClickThrough = false);
	
	bool shouldSelect(d2d::Rect rc, Vec2 const& pt) override;

	void drawColorPicker();

	void jumpToModule(std::string const& name) {
		jumpModule = name;
		modTab = ALL;
	}
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
		ValueType rgbSelector = BoolValue(false);

		Setting rgbSetting = Setting{ "colorpickerrgb", "RGB", "Cycle through a rainbow of colors for this setting" };

		ColorPicker() {
			rgbSetting.value = &rgbSelector;
		}
	} colorPicker{};

	ui::TextBox searchTextBox{};
	std::vector<ui::TextBox> pickerTextBoxes{};

	ComPtr<ID2D1Bitmap1> shadowBitmap;
	ComPtr<ID2D1Bitmap1> auxiliaryBitmap;
	ComPtr<ID2D1Bitmap1> modHoverBitmap;
	ComPtr<ID2D1ImageBrush> clipBrush;
	std::optional<d2d::Rect> modClip = {};

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
		std::wstring name;
		std::wstring description;
		std::wstring pluginName = L"";
		std::shared_ptr<class IModule> mod;
		bool shouldRender = true;
		bool isExtended = false;
		bool isMarketScript = false;
		Vec2 previewSize = {};
		float arrowRot = 180.f;
		float lerpArrowRot = 1.f;
		float lerpToggle = 0.f;
		float lerpHover = 0.f;
		Color toggleColorOn = {};
		Color toggleColorOff = {};
		std::optional<d2d::Rect> modRect = std::nullopt;

		static bool compare(ModContainer const& a, ModContainer const& b) {
			return (a.name < b.name) || (!a.isMarketScript && b.isMarketScript);
		}
	};

	std::map<Setting*, std::shared_ptr<ui::TextBox>> settingBoxes = {};

	d2d::Rect rect = {};
	d2d::Rect cPickerRect = {};

	Setting* activeSetting = nullptr;
	int capturedKey = 0;
	float adaptedScale = 0.f;

	float scrollMax = 0.f;
	float scroll = 0.f;
	float lerpScroll = 0.f;

	std::optional<std::string> jumpModule;
	ComPtr<ID2D1Effect> compositeEffect;
};