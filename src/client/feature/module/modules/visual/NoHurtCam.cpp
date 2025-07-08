#include "pch.h"
#include "NoHurtCam.h"

NoHurtCam::NoHurtCam() : Module("NoHurtCam", LocalizeString::get("client.module.noHurtCam.name"), LocalizeString::get("client.module.noHurtCam.desc"), GAME) {
	listen<BobHurtEvent>(static_cast<EventListenerFunc>(&NoHurtCam::onBobHurt));
}

void NoHurtCam::onBobHurt(Event& ev) {
	reinterpret_cast<BobHurtEvent&>(ev).setCancelled();
}
