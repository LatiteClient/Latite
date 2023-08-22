/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "api/feature/command/CommandManager.h"
#include "client/feature/command/script/JsCommand.h"
#include "Command.h"

class CommandManager final : public ICommandManager {
public:
	std::string prefix = ".";

	CommandManager();
	virtual ~CommandManager() = default;

	bool runCommand(std::string const& line);
	bool registerScriptCommand(JsCommand* cmd) {
		for (auto& mod_ : items) {
			if (mod_->name() == cmd->name()) {
				return false;
			}
		}
		this->items.push_back(std::shared_ptr<JsCommand>(cmd));
		return true;
	}

	bool deregisterScriptCommand(JsCommand* cmd) {
		for (auto it = items.begin(); it != items.end(); it++) {
			if (it->get() == cmd) {
				items.erase(it);
				return true;
			}
		}
		return false;
	}
};