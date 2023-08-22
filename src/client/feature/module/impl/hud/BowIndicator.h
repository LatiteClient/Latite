/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../../TextModule.h"

class BowIndicator : public TextModule {
public:
	BowIndicator();
	~BowIndicator();

private:
	ValueType visual = BoolValue(false);
	ValueType horizontal = BoolValue(false);
	ValueType indicatorSize = FloatValue(40.f);
	ValueType indicatorWidth = FloatValue(10.f);
	ValueType indicatorCol = ColorValue({ 1.f, 1.f, 1.f, 1.f });
	ValueType indicatorCol2 = ColorValue({ 0.1f, 0.1f, 0.7f, 0.8f });
	ValueType indicatorRad = FloatValue(5.f);

	void render(DXContext& dc, bool isDefault, bool inEditor) override;
	std::wstringstream text();
};