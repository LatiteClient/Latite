#pragma once
#include "Module.h"
#include "util/DxContext.h"
#include "json/json.hpp"

class HUDModule : public Module {
public:
	HUDModule(std::string const& name, std::string const& displayName, std::string const& description, Category category, int keybind = 0,
		bool resizable = true)
		: Module(name, displayName, description, category, keybind, true), resizable(resizable) {}

	virtual void render(DXContext& ctx, bool isDefault, bool inEditor) = 0;

	virtual void renderSelected();
	virtual void renderFrame();
	virtual void renderPost();

	void setPos(Vec2 newPos) {
		rect.setPos(newPos);
	}

	[[nodiscard]] float getScale() { return scale; }
	[[nodiscard]] d2d::Rect getRect() { return rect; }
	
	void setScale(float f) { scale = f; }
protected:
	d2d::Rect rect = {};
	float scale = 1.f;
	bool resizable;
public:
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
