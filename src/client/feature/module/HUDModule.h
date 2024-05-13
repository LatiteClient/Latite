#pragma once
#include "Module.h"
#include "util/DxContext.h"
#include "json/json.hpp"

class HUDModule : public Module {
public:
	static constexpr float min_scale = 0.f;
	static constexpr float max_scale = 4.f;

	HUDModule(std::string const& name, std::string const& displayName, std::string const& description, Category category, int keybind = 0,
		bool resizable = true)
		: Module(name, displayName, description, category, keybind, true), resizable(resizable) {
		
		addSetting("pos", "Position", "<internal setting>", storedPos);
		addSetting("snapX", "Snap X", "<internal setting>", snappingX);
		addSetting("snapY", "Snap Y", "<internal setting>", snappingY);

		if (resizable) addSliderSetting("scale", "Size", "", scale, FloatValue(min_scale), FloatValue(max_scale), FloatValue(0.05f));
		addSetting("forceMinecraftRend", "Force Minecraft Renderer", "Whether or not to use the Minecraft renderer for this module.", forceMCRend);
	}

	virtual ~HUDModule() = default;

	virtual void render(DrawUtil& ctx, bool isDefault, bool inEditor) = 0;
	virtual void preRender(bool mcRend, bool isDefault, bool inEditor) {};

	virtual void renderSelected(DrawUtil& dc);
	virtual void renderFrame(DrawUtil& dc);
	virtual void renderPost(DrawUtil& dc);
	void afterLoadConfig() override;
	void loadStoredPosition();
	void storePos(Vec2 const& ss);

	void setPos(Vec2 newPos) {
		rect.setPos(newPos);
	}

	[[nodiscard]] float getScale() { return std::get<FloatValue>(scale); }
	[[nodiscard]] d2d::Rect getRect();
	[[nodiscard]] d2d::Rect getRectNonScaled() { return rect; }
	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return std::get<BoolValue>(forceMCRend); }
	[[nodiscard]] bool isResizable() { return resizable; }
	[[nodiscard]] bool isShowPreview() { return showPreview; }

	void setScale(float f) { std::get<FloatValue>(scale) = f; }
	void setRect(d2d::Rect const& rc) { this->rect = rc; }


protected:
	d2d::Rect rect = {};
	ValueType storedPos = Vec2Value();
	ValueType scale = FloatValue(1.f);
	ValueType forceMCRend = BoolValue(false);
	bool resizable;
	bool active = true;
	bool showPreview = true;
public:
	[[nodiscard]] virtual bool isActive() { return active; }
	virtual void loadConfig(SettingGroup& resolvedGroup) override;

	ValueType snappingX = SnapValue{};
	ValueType snappingY = SnapValue{};
};
