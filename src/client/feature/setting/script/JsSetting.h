/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/feature/setting/Setting.h"
#include "util/ChakraUtil.h"

class JsSetting : public Setting {
public:
	JsSetting(std::string const& internalName, std::string const& displayName, std::string const& description)
		: Setting(internalName, displayName, description) {
		this->value = &jsVal;
	}

	ValueType jsVal = BoolValue(false);
};