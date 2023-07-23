#pragma once
#include <string>
#include "api/eventing/Listenable.h"
#include <vector>
#include "util/DXUtil.h"

class Screen : public Listener {
public:
	Screen(std::string const& name) : name(name) {}

	~Screen() = default;
	Screen(Screen&) = delete;
	Screen(Screen&&) = delete;

	[[nodiscard]] bool isActive() { return active; }
	void setActive(bool b) { this->active = b; }

	[[nodiscard]] bool shouldListen() override { return isActive(); }
	[[nodiscard]] std::string getName() { return name; }

	// TODO: grabMouse and releaseMouse
	virtual void onEnable() {};
	virtual void onDisable() {};
	
	void setLayer(int layer) {
		this->currentLayer = layer;
	}
	
	void addLayer(d2d::Rect const& rc) {
		layers.push_back(rc);
	}

	void clearLayers() {
		layers.clear();
		layers.push_back({});
	}

	[[nodiscard]] bool shouldSelect(d2d::Rect rc, Vec2 const& pt) {
		for (int i = 0; i < layers.size(); i++) {
			if (i >= currentLayer || layers[i].contains(pt)) return false;
		}
		return rc.contains(pt);
	}
private:
	int currentLayer = 0;
	std::vector<d2d::Rect> layers;
	bool active = false;
	std::string name;
};