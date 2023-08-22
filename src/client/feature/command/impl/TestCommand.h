/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Command.h"

class TestCommand final : public Command {
public:
	TestCommand();
	~TestCommand() = default;

	bool execute(std::string const label, std::vector<std::string> args) override;
};