#include "CommandManager.h"
#include "util/logger.h"

void CommandManager::init()
{
	// add commands here
}

bool CommandManager::runCommand(std::string const& line)
{
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
							//ClientMessageNF(TextFormat::Format(TextFormat::RED) << "Usage: " << prefix << newArgs[0] << " " << cmd->usage);
						}
						return result;
					}
					catch (std::exception& e) {
						Logger::warn("An unhandled exception occured while running this command: {}", e.what());
						//ClientMessageF("An unhandled exception occured while running this command: " << e.what());
						return false;
					}
				}
			}
		}

	//ClientMessageF("Unknown command: " << (newArgs.empty() ? "" : newArgs[0]) << ".");
	return false;
}
