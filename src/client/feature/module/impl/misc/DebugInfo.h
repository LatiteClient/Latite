#pragma once
#include "../../Module.h"
#include "util/DxUtil.h"

class DebugInfo : public Module {
public:
	DebugInfo();
private:
	void onRenderOverlay(Event& ev);
	void onRenderHUDModules(Event& ev);
};
