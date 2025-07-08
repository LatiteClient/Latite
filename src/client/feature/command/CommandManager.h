#pragma once
#include "client/feature/command/script/JsCommand.h"
#include "Command.h"

class CommandManager final : public Manager<ICommand>  {
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
		JS::JsAddRef(cmd->obj, nullptr);
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