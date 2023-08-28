#include "HelpCommand.h"
#include <sstream>
#include "client/latite.h"
#include "client/feature/command/CommandManager.h"
#include "pch.h"

HelpCommand::HelpCommand() : Command("help", "Shows all commands and descriptions.", "{0}", {"?", ""}) {
}

bool HelpCommand::execute(std::string const label, std::vector<std::string> args) {
    std::stringstream ss;

    ss << "List of all commands:";
    Latite::getCommandManager().forEach([&](std::shared_ptr<ICommand> cmd) {
        ss << "\n " << util::Format("&7" + cmd->name() + "&r") << ": " << cmd->desc();
        /*
        if (cmd->getAliases().size() > 1) {
            ss << " (Aliases: ";
            for (size_t i = 1; i < cmd->getAliases().size(); i++) {
                auto& alias = cmd->getAliases()[i] ;
                ss << alias;
                if (cmd->getAliases().size() - 1 < i) {
                    ss << ", ";
                }
            }
            ss << ")";
        }*/
        });

    message(ss.str());
    return true;
}
