#include "pch.h"
#include "NoHurtCam.h"

NoHurtCam::NoHurtCam() : Module("NoHurtCam", "No Hurt Cam", "Disable camera shake when hurt.", GAME) {
}

void NoHurtCam::onBobHurt(Event& ev) {
	reinterpret_cast<BobHurtEvent&>(ev).setCancelled();
}
