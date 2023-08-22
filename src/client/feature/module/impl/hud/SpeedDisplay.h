/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../../TextModule.h"

class SpeedDisplay : public TextModule {
public:
	SpeedDisplay();
private:
	void onTick(Event& ev);

	ValueType decimals = FloatValue(3.f);
	ValueType includeY = BoolValue(false);

	float speed;
protected:
	std::wstringstream text(bool isDefault, bool inEditor) override;
};