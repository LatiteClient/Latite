#include "pch.h"
#include "PingDisplay.h"
#include "mc/common/network/RakNetConnector.h"
#include "client/event/events/AveragePingEvent.h"
#include "client/event/Eventing.h"

PingDisplay::PingDisplay() : TextModule("PingDisplay", LocalizeString::get("client.textmodule.pingDisplay.name"), LocalizeString::get("client.textmodule.pingDisplay.desc"), HUD) {
	this->suffix = TextValue(L" ms");

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
