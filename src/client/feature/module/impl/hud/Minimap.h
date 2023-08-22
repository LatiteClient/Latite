/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

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