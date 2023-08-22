/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "Hook.h"
#include "api/manager/Manager.h"

class LatiteHooks final : public Manager<HookGroup> {
public:
	LatiteHooks() = default;
	~LatiteHooks() = default;

	void init();
	void uninit();

	void enable();
	void disable();
private:
};