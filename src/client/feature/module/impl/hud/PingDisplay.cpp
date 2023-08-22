/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "PingDisplay.h"
#include "sdk/common/network/RakNetConnector.h"
#include "client/event/impl/AveragePingEvent.h"
#include "client/event/Eventing.h"

PingDisplay::PingDisplay() : TextModule("PingDisplay", "Ping Display", "Displays the ping of the server you're connected to", HUD) {
	this->suffix = TextValue(" MS");

	listen<AveragePingEvent>((EventListenerFunc)&PingDisplay::onAvgPing, true);
}

std::wstringstream PingDisplay::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	int dPing = 0;

	auto inst = SDK::RakNetConnector::get();
	if (inst && inst->getPeer()) {
		dPing = ping;
	}

	wss << dPing;

	return wss;
}

void PingDisplay::onAvgPing(Event& evGeneric) {
	AveragePingEvent& ev = static_cast<AveragePingEvent&>(evGeneric);

	ping = ev.getPing();
}
