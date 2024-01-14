#pragma once
#include "../../Module.h"

class BlockOutline : public Module {
public:
	BlockOutline();

	void onOutlineSelection(Event& evG);
private:
	ValueType renderThrough = BoolValue(false);
	ValueType transparent = BoolValue(true);
	ValueType outlineVanilla = BoolValue(false);

	ValueType outline = BoolValue(true);
	ValueType outlineColor = ColorValue(0.f, 0.f, 0.f);

	ValueType overlay = BoolValue(false);
	ValueType faceOverlay = BoolValue(false);
	ValueType overlayColor = ColorValue(1.f, 1.f, 1.f, 0.5f);
};

