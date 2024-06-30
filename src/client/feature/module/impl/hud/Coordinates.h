#pragma once
#include "../../HUDModule.h"
#include <sdk/common/client/gui/controls/UIControl.h>

class Coordinates : public HUDModule {
public:
	Coordinates();

	void render(DrawUtil& dc, bool, bool) override;
	void onRenderLayer(Event& ev);
	void updatePos();
private:
	SDK::UIControl* vanillaCoordinates;
	ValueType showDimension = BoolValue(true);
	ValueType hideVanillaCoordinates = BoolValue(false);
	ValueType movableVanillaCoordinates = BoolValue(false);
};
