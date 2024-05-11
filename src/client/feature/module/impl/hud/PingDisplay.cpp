#include "pch.h"
#include "PingDisplay.h"
#include "sdk/common/network/RakNetConnector.h"
#include "client/event/impl/AveragePingEvent.h"
#include "client/event/Eventing.h"

PingDisplay::PingDisplay() : TextModule("PingDisplay", "Ping Display", "Displays the average upstream ping of the connected server.", HUD) {
	this->suffix = TextValue(L" MS");

	listen<AveragePingEvent>((EventListenerFunc)&PingDisplay::onAvgPing, true);
}

std::wstringstream PingDisplay::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	int dPing = 0;

	auto inst = SDK::RakNetConnector::get();
	if (inst) {
		dPing = ping;
	}

	wss << dPing;

	return wss;
}

void PingDisplay::onAvgPing(Event& evGeneric) {
	AveragePingEvent& ev = static_cast<AveragePingEvent&>(evGeneric);

	ping = ev.getPing();
}
