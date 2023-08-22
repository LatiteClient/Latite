/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../../Module.h"
#include "client/event/impl/GammaEvent.h"

class HurtColor : public Module {
public:
	HurtColor();

	void onActorOverlay(Event& ev);
private:
	ValueType color = ColorValue(1.f, 0.f, 0.f, 0.6f);
};