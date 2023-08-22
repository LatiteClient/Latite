/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../../Module.h"
#include "client/event/impl/GammaEvent.h"

class Fullbright : public Module {
public:
	Fullbright();

	void onGamma(Event& ev);
private:
	ValueType gamma = FloatValue(25.f);
};