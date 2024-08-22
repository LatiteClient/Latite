#include "pch.h"
#include "CinematicCamera.h"

CinematicCamera::CinematicCamera() : Module("CinematicCamera",
                                            LocalizeString::get("client.module.cinematicCamera.name"),
                                            LocalizeString::get("client.module.cinematicCamera.desc"), GAME, VK_F4) {
    listen<CinematicCameraEvent>(static_cast<EventListenerFunc>(&CinematicCamera::onCinematicCamera));
}

void CinematicCamera::onCinematicCamera(Event& evGeneric) {
	auto& ev = reinterpret_cast<CinematicCameraEvent&>(evGeneric);
	ev.setValue(true);
}