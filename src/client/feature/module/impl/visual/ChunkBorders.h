#pragma once
#include "../../Module.h"

class ChunkBorders : public Module
{
	ValueType transparent = BoolValue(true);
public:
	ChunkBorders();

	void onRender3d(Event& evG);
};

