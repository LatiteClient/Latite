#pragma once
#include "../../HUDModule.h"
#include "util/Math.h"
#include <unordered_map>

class Minimap : public HUDModule {
public:
	Minimap();

private:
	struct IBlock {

	};

	std::unordered_map<BlockPos, 

	void scanChunks();
	void onUpdate(Event& ev);
};