#pragma once
#include "../../TextModule.h"
#include <sdk/common/client/gui/controls/UIControl.h>

class Coordinates : public TextModule {
public:
	Coordinates();

	std::wstringstream text(bool isDefault, bool inEditor) override;
	void onRenderLayer(Event& ev);
	void updatePos();
private:
	SDK::UIControl* vanillaCoordinates;
	ValueType showDimension = BoolValue(false);
	ValueType hideVanillaCoordinates = BoolValue(false);
	ValueType movableVanillaCoordinates = BoolValue(false);
};
