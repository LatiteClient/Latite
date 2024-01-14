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
		
		addSetting("pos", "Position", "", storedPos);
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
	void storePos(Vec2 const& ss);

	void setPos(Vec2 newPos) {
		rect.setPos(newPos);
	}

	[[nodiscard]] float getScale() { return std::get<FloatValue>(scale); }
	[[nodiscard]] d2d::Rect getRect();
	[[nodiscard]] d2d::Rect getRectNonScaled() { return rect; }
	[[nodiscard]] virtual bool forceMinecraftRenderer() override { return std::get<BoolValue>(forceMCRend); }
	
	void setScale(float f) { std::get<FloatValue>(scale) = f; }
	void setRect(d2d::Rect const& rc) { this->rect = rc; }

	bool isResizable() { return resizable; }

protected:
	d2d::Rect rect = {};
	ValueType storedPos = Vec2Value();
	ValueType scale = FloatValue(1.f);
	ValueType forceMCRend = BoolValue(false);
	bool resizable;
	bool active = true;
public:
	[[nodiscard]] virtual bool isActive() { return active; }

	struct Snapping {
		enum Type : int {
			Normal,
			MCUI,
			Module
		};

		enum Pos : int {
			Right,
			Middle,
			Left
		};

		bool doSnapping = false;
		Type type = Normal;
		Pos pos = Pos::Right;
		std::string mod = "";
		int idx = 0;

		void snap(Type type, Pos pos, int idx, std::string mod = "") {
			this->doSnapping = true;
			this->type = type;
			this->pos = pos;
			this->mod = mod;
			this->idx = idx;
		}

		nlohmann::json getJSON() {
			nlohmann::json j = nlohmann::json::object();
			if (doSnapping) {
				j["type"] = type;
				if (type == Module) {
					j["module"] = mod;
				}
				j["idx"] = idx;
				j["pos"] = pos;
			}
			return j;
		}

		void fromJSON(nlohmann::json& j) {
			doSnapping = false;
			if (j.contains("type")) {
				doSnapping = true;
				this->type = j["type"].get<Type>();
				if (this->type == Module) {
					this->mod = j["module"].get<std::string>();
				}
				this->idx = j["idx"].get<int>();
				this->pos = j["pos"].get<Pos>();
			}
		}
	};

	Snapping snappingX = {};
	Snapping snappingY = {};
};
