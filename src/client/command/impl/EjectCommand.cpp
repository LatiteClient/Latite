#include "EjectCommand.h"
#include "client/Latite.h"


EjectCommand::EjectCommand() : Command("eject", "Removes Latite from the game.", "")
{
}

bool EjectCommand::execute(std::string const label, std::vector<std::string> args)
{
	Latite::get().doEject();
	return false;
}
