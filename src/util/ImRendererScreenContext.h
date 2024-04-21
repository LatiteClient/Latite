#pragma once
#include <imgui/imgui.h>

namespace ImRendererScreenContext {
	extern void init();
	extern void reset();
	extern ImDrawList* getDrawList();
	extern void renderDrawList(SDK::ScreenContext* screenContext, ImDrawList* data);
}