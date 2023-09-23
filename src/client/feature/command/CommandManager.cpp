#include "pch.h"
#include "CommandManager.h"
#include "util/logger.h"
#include "client/latite.h"
#include "client/misc/ClientMessageSink.h"
#include "util/Util.h"

// Commands
#include "impl/TestCommand.h"
#include "impl/HelpCommand.h"
#include "impl/EjectCommand.h"
#include "impl/ToggleCommand.h"
#include "impl/ScriptCommand.h"
#include "impl/SetPrefixCommand.h"
#include "impl/ConfigCommand.h"
#include "impl/SignCommand.h"
//

CommandManager::CommandManager() {
#if LATITE_DEBUG
	this->items.push_back(std::make_shared<TestCommand>());
#endif
	this->items.push_back(std::make_shared<HelpCommand>());
	this->items.push_back(std::make_shared<EjectCommand>());
	this->items.push_back(std::make_shared<ToggleCommand>());
	this->items.push_back(std::make_shared<ScriptCommand>());
	this->items.push_back(std::make_shared<SetPrefixCommand>());
	this->items.push_back(std::make_shared<ConfigCommand>());
#if LATITE_DEBUG
	this->items.push_back(std::make_shared<SignCommand>());
#endif
}

bool CommandManager::runCommand(std::string const& line) {
	if (line.substr(0, prefix.size()) != prefix) {
		return false;
	}
	std::string myLine = line.substr(prefix.size());

	bool isQuoted = false;
	bool isEscaped = false;
	std::vector<std::string> newArgs = {};


	std::string word = "";

	// parser
	for (size_t i = 0; i < myLine.size(); i++) {
		char ch = myLine[i];
		if (ch == '\\' && !isEscaped) {
			isEscaped = true;
			continue;
		}
		if (!isEscaped) {
			if (ch == '"') {
				if (isQuoted) {
					newArgs.push_back(word);
					word = "";
				}
				isQuoted = !isQuoted;
				continue;
			}
		}
		if (isQuoted) {
			word += ch;
			continue;
		}
		if (!isEscaped) {
			if (ch == ' ' || myLine.size() - i < 2) {
				if (myLine.size() - i < 2) {
					word += ch;
				}
				if (!word.empty()) {
					newArgs.push_back(word);
					word = "";
				}
				continue;
			}
		}

		word += ch;
		isEscaped = false;
	}

	// Sort all commands alphabetically
	std::sort(items.begin(), items.end(), [](std::shared_ptr<ICommand>& left, std::shared_ptr<ICommand>& right) {
		return left->name() < right->name();
		});
	
	if (!newArgs.empty()) {
		std::string label = util::ToLower(newArgs[0]);
		for (auto& cmd : this->items) {
			for (std::string alias : cmd->getAliases()) {
				if (alias == label /*label*/) {
					std::vector<std::string> args;
					for (size_t k = 1; k < newArgs.size(); k++) {
						args.push_back(newArgs[k]);
					}
					try {
						bool result = cmd->tryRun(label, args, myLine);
						if (!result) {
							std::string usage = cmd->getUsage();

							size_t pos = 0;
							while ((pos = usage.find("$", pos)) != std::string::npos) {
								usage.replace(pos, strlen("$"), prefix + label);
								pos += strlen("$");
							}

							Latite::getClientMessageSink().push(util::Format("&cUsage: " + usage));
						}
						return result;
					}
					catch (std::exception& e) {
						Logger::Warn("An unhandled exception occured while running this command: {}", e.what());
						Latite::getClientMessageSink().push(util::Format(std::string("&cAn unhandled exception occured while running this command: ") + e.what()));
						return false;
					}
				}
			}
		}
	}
	else {
		runCommand(prefix + "help");
		return false;
	}

	Latite::getClientMessageSink().push(util::Format("&cUnknown command: " + (newArgs.empty() ? "" : newArgs[0]) + "."));
	return false;
}
