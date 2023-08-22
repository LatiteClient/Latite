/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

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
