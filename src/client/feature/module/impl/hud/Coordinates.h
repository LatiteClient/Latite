#pragma once
#include "../../HUDModule.h"

class Coordinates : public HUDModule {
public:
	Coordinates();

	void render(DrawUtil& dc, bool, bool) override;
private:
	ValueType showDimension = BoolValue(true);
};