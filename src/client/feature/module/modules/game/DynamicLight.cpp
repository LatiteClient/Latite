#include "pch.h"
#include "DynamicLight.h"
#include "client/event/Eventing.h"
#include "client/event/events/TickEvent.h"

DynamicLight::DynamicLight() : Module("DynamicLight", L"Dynamic Light", L"Makes the item you're holding emit light.", GAME) {
	listen<TickEvent>((EventListenerFunc)&DynamicLight::onTick, true);
}

DynamicLight::~DynamicLight() {
}

void DynamicLight::onTick(Event&) {
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;
	
}
