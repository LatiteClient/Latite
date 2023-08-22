/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Command.h"
#include "client/script/feature/JsEvented.h"

class JsCommand : public Command, public JsEvented {
public:
	JsCommand(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: Command(name, description, usage, aliases) {
		JS::JsGetCurrentContext(&ctx);
		this->eventListeners[L"execute"] = {};
		script = true;
	}

	virtual bool execute(std::string const label, std::vector<std::string> args) override;

	JsContextRef ctx;
private:
};