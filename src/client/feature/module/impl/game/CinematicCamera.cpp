#include "pch.h"
#include "CinematicCamera.h"

CinematicCamera::CinematicCamera() : Module("CinematicCamera", "Cinematic Camera", "Makes your camera smooth and cinematic", GAME, VK_F4) {
	listen<CinematicCameraEvent>((EventListenerFunc)&CinematicCamera::onCinematicCamera);
}

void CinematicCamera::onCinematicCamera(Event& evGeneric) {
	auto& ev = reinterpret_cast<CinematicCameraEvent&>(evGeneric);
	ev.setValue(true);
}