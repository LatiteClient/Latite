#pragma once
#include <string>
#include "api/eventing/Listenable.h"
#include "api/eventing/Event.h"
#include "api/feature/setting/Setting.h"
#include "client/event/impl/RenderGameEvent.h"
#include <vector>
#include "util/DXUtil.h"
#include <optional>

class Screen : public Listener {
public:
	Screen(std::string const& name);

	~Screen() = default;
	Screen(Screen&) = delete;
	Screen(Screen&&) = delete;

	[[nodiscard]] bool isActive() { return active; }
	void setActive(bool b) { this->active = b; }

	[[nodiscard]] bool shouldListen() override { return isActive(); }
	[[nodiscard]] std::string getName() { return name; }

	// TODO: grabMouse and releaseMouse
	virtual void onEnable(bool ignoreAnimations = false) {};
	virtual void onDisable() {};
	
	void setLayer(int layer) {
		this->currentLayer = layer;
	}
	
	void addLayer(d2d::Rect const& rc) {
		layers.push_back(rc);
	}

	void clearLayers() {
		layers.clear();
	}

	[[nodiscard]] bool shouldClose() { return closing; }

	[[nodiscard]] virtual bool shouldSelect(d2d::Rect rc, Vec2 const& pt) {
		for (int i = 0; i < layers.size(); i++) {
			if (layers[i].contains(pt)) return false;
		}
		return rc.contains(pt);
	}

	enum class Cursor {
		Arrow,
		Hand,
		IBeam
	} cursor = Cursor::Arrow;

	KeyValue key = KeyValue();
protected:
	HCURSOR arrow;
	HCURSOR hand;
	HCURSOR ibeam;
	virtual void close();
	std::array<bool, 3> mouseButtons = {};
	std::array<bool, 3> activeMouseButtons = {};
	std::array<bool, 3> justClicked = {};

	std::optional<std::wstring> tooltip;
	void playClickSound();
private:

	bool closing = false;

	void onUpdate(Event& ev);
	void onClick(Event& ev);
	void onRenderOverlay(Event& ev);

	int currentLayer = 0;
	std::vector<d2d::Rect> layers;
	bool active = false;
	std::string name;
};