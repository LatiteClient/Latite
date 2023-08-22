#include "EjectCommand.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"

EjectCommand::EjectCommand() : Command("eject", "Removes Latite from the game.", "{0}") {
}

bool EjectCommand::execute(std::string const label, std::vector<std::string> args) {
	message("Unloaded the client.");
	Latite::get().queueEject();
	return true;
}
