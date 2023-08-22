/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include <string>
#include "util/ChakraUtil.h"

class JsLibrary {
public:
	JsLibrary(class JsScript* owner, std::wstring const& inclName) : includeName(inclName), owner(owner) {}

	virtual JsValueRef initialize(JsValueRef parentObj) = 0;
	virtual bool shouldInclude(std::wstring const& str) {
		return this->includeName == str;
	}
protected:
	class JsScript* owner;
	std::wstring includeName;
};
