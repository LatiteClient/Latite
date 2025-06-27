#pragma once

#include "../../Module.h"

class Waypoints : public Module {
public:
	Waypoints();
private:
	void onRenderOverlay(Event& ev);

	std::optional<Vec2> smoothedScreenPos = std::nullopt;
};