#pragma once
#include "Module.h"
#include "util/DxContext.h"
#include "json/json.hpp"

class HUDModule : public Module {
public:
	HUDModule(std::string const& name, std::string const& displayName, std::string const& description, Category category, int keybind = 0,
		bool resizable = true)
		: Module(name, displayName, description, category, keybind, true), resizable(resizable) {
		
		addSetting("pos", "Position", "", storedPos);
		addSliderSetting("scale", "Size", "", scale, FloatValue(0.f), FloatValue(4.f), FloatValue(0.05f));
	}

	virtual void render(DXContext& ctx, bool isDefault, bool inEditor) = 0;

	virtual void renderSelected();
	virtual void renderFrame();
	virtual void renderPost();
	void afterLoadConfig() override;
	void storePos();

	void setPos(Vec2 newPos) {
		rect.setPos(newPos);
	}

	[[nodiscard]] float getScale() { return std::get<FloatValue>(scale); }
	[[nodiscard]] d2d::Rect getRect();
	
	void setScale(float f) { std::get<FloatValue>(scale) = f; }
	void setRect(d2d::Rect const& rc) { this->rect = rc; }

protected:
	d2d::Rect rect = {};
	ValueType storedPos = Vec2Value();
	ValueType scale = FloatValue(1.f);
	bool resizable;
	bool active = true;
public:
	[[nodiscard]] bool isActive() { return active; }

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
