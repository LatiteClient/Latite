#include "FPSCounter.h"
#include "client/Latite.h"

FPSCounter::FPSCounter() : TextModule("FPS", "FPS Counter", "Shows your FPS.", HUD) {
}

std::wstringstream FPSCounter::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	wss << Latite::get().getTimings().getFPS();
	return wss;
}
