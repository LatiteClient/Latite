#pragma once
#include "../../Module.h"

class Hitboxes : public Module {
public:

	EnumData mode;
	static constexpr int mode_bounding_box = 0;
	static constexpr int mode_hitbox = 1;

	ValueType boxColor = ColorValue(1.f, 1.f, 1.f, 1.f);

	ValueType showEyeLine = BoolValue(true);
	ValueType eyeColor = ColorValue(1.f, 0.f, 0.f, 1.f);

	ValueType showLine = BoolValue(true);
	ValueType lineColor = ColorValue(0.f, 0.f, 1.f, 1.f);

	ValueType showHelper = BoolValue(false);
	ValueType helperLineColor = ColorValue(1.f, 0.f, 1.f, 1.f);

	ValueType localPlayer = BoolValue(true);
	ValueType items = BoolValue(true);

	Hitboxes();
	virtual ~Hitboxes() = default;
};