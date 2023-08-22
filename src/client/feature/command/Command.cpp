/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "Command.h"
#include "pch.h"

#include "client/misc/ClientMessageSink.h"
#include "client/Latite.h"
#include "util/Util.h"

/*
Command::Command(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases)
	: cmdName(name), description(description), usage(usage) {
	this->aliases = { name };
	for (auto& a : aliases) {
		this->aliases.push_back(a);
	}
}*/

void Command::message(std::string const& str, bool error) {
	std::string fin = "";
	if (!error) {
		fin = util::Format("[&9Latite&r] " + util::Format(str));
	}
	else {
		fin = util::Format("&c" + util::Format(str));
	}
	Latite::get().getClientMessageSink().display(fin);
}
