#include "DynamicLight.h"
#include "client/event/Eventing.h"
#include "client/event/impl/TickEvent.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/player/LocalPlayer.h"

DynamicLight::DynamicLight() : Module("DynamicLight", "Dynamic Light", "Makes the item you're holding emit light", GAME) {
	listen<TickEvent>((EventListenerFunc)&DynamicLight::onTick, true);
}

DynamicLight::~DynamicLight() {
}

void DynamicLight::onTick(Event&) {
	auto plr = SDK::ClientInstance::get()->getLocalPlayer();
	if (!plr) return;
	
}
