#include "CommandManager.h"
#include "util/logger.h"
#include "client/latite.h"
#include "client/misc/ClientMessageSink.h"
#include "pch.h"
#include "util/util.h"

// Commands
#include "impl/TestCommand.h"
#include "impl/HelpCommand.h"
#include "impl/EjectCommand.h"
#include "impl/ToggleCommand.h"
//

CommandManager::CommandManager() {
#if LATITE_DEBUG
	this->items.push_back(std::make_shared<TestCommand>());
#endif
	this->items.push_back(std::make_shared<HelpCommand>());
	this->items.push_back(std::make_shared<EjectCommand>());
	this->items.push_back(std::make_shared<ToggleCommand>());
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

	if (!newArgs.empty())
		for (auto& cmd : this->items) {
			for (std::string alias : cmd->getAliases()) {
				if (alias == newArgs[0] /*label*/) {
					std::vector<std::string> args;
					for (size_t k = 1; k < newArgs.size(); k++) {
						args.push_back(newArgs[k]);
					}
					try {
						bool result = cmd->execute(newArgs[0], args);
						if (!result) {
							// TODO: Send message
							Latite::get().getClientMessageSink().push(util::format("&cUsage: " + prefix + newArgs[0] + " " + cmd->getUsage()));
						}
						return result;
					}
					catch (std::exception& e) {
						Logger::warn("An unhandled exception occured while running this command: {}", e.what());
						Latite::get().getClientMessageSink().push(util::format(std::string("&cAn unhandled exception occured while running this command: ") + e.what()));
						return false;
					}
				}
			}
		}
	else {
		runCommand(".help");
		return false;
	}

	Latite::get().getClientMessageSink().push(util::format("&cUnknown command: " + (newArgs.empty() ? "" : newArgs[0]) + "."));
	return false;
}
