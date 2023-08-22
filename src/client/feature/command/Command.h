/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include <string>
#include <vector>
#include "api/feature/command/Command.h"

class Command : public ICommand {
public:
	~Command() = default;
	Command(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: ICommand(name, description, usage, aliases) {}

	virtual void message(std::string const& str, bool error = false) override;
protected:
};
