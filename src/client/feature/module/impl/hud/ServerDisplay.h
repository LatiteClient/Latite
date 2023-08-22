/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../../TextModule.h"

class ServerDisplay : public TextModule {
private:
	static constexpr int port_mixed = 0;
	static constexpr int port_constant = 1;
	static constexpr int port_none = 2;
	EnumData port;

	ValueType showServerName = BoolValue(true);
public:
	ServerDisplay();
	~ServerDisplay() = default;
protected:

	std::wstringstream text(bool isDefault, bool inEditor) override;

};