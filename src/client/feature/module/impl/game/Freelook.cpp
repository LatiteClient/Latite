#include "pch.h"
#include "Freelook.h"

Freelook::Freelook() : Module("Freelook", "Freelook", "Look around freely.", GAME, 'F') {
	listen<UpdatePlayerCameraEvent>((EventListenerFunc)&Freelook::onCameraUpdate);
	listen<PerspectiveEvent>((EventListenerFunc)&Freelook::onPerspective);
}

void Freelook::onCameraUpdate(Event& evG) {
	auto& ev = reinterpret_cast<UpdatePlayerCameraEvent&>(evG);

	ev.setViewAngles(lastRot);
}

void Freelook::onPerspective(Event& evG) {
	auto& ev = reinterpret_cast<PerspectiveEvent&>(evG);
	ev.getView() = 1;
}

void Freelook::onEnable() {
	lastRot = SDK::ClientInstance::get()->getLocalPlayer()->getRot();
}

void Freelook::onDisable() {
}
