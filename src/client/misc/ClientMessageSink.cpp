/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "ClientMessageSink.h"
#include "util/Logger.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "pch.h"

void ClientMessageSink::doPrint(int numMessages) {
	sinkLock.lock();
	int msgs = 0;
	for (auto it = messages.begin(); it != messages.end();) {
		if (msgs >= numMessages) break;

		auto& msg = *it;
		auto cInst = SDK::ClientInstance::get();
		auto lp = cInst->getLocalPlayer();
		if (lp) {
			lp->displayClientMessage(msg);
			messages.erase(it);
			continue;
		}
		++it;
		msgs++;
	}
	sinkLock.unlock();
}

void ClientMessageSink::push(std::string const& message) {
	this->sinkLock.lock();
	this->messages.push_back(message);
	this->sinkLock.unlock();
}

void ClientMessageSink::display(std::string const& message) {
	auto cInst = SDK::ClientInstance::get();
	auto lp = cInst->getLocalPlayer();
	if (lp) {
		lp->displayClientMessage(message);
	}
}
