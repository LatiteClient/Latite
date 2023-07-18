#include "EjectCommand.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"

EjectCommand::EjectCommand() : Command("eject", "Removes Latite from the game.", "") {
}

bool EjectCommand::execute(std::string const label, std::vector<std::string> args) {
	message("Unloading the client..");
	Latite::get().queueEject();
	return true;
}
