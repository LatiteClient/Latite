#pragma once
#include "../../Module.h"

class DebugInfo : public Module {
public:
	DebugInfo();
	virtual ~DebugInfo() = default;

private:
	void onRenderOverlay(Event& ev);
	void onRenderHUDModules(Event& ev);
};
