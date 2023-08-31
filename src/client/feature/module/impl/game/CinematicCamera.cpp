#include "pch.h"
#include "CinematicCamera.h"

CinematicCamera::CinematicCamera() : Module("CinematicCamera", "Cinematic Camera", "like the one in java", GAME, VK_F5) {
	listen<CinematicCameraEvent>((EventListenerFunc)&CinematicCamera::onCinematicCamera);
}

void CinematicCamera::onCinematicCamera(Event& evGeneric) {
	auto& ev = reinterpret_cast<CinematicCameraEvent&>(evGeneric);
	ev.setValue(true);
}