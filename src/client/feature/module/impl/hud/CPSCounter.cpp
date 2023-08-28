#include "pch.h"
#include "CPSCounter.h"
#include "client/Latite.h"

CPSCounter::CPSCounter() : TextModule("CPS", "CPS Counter", "Shows your CPS", HUD) {
	mode.addEntry(EnumEntry(0, "Left", "Left"));
	mode.addEntry(EnumEntry(1, "Right", "Right"));
	mode.addEntry(EnumEntry(2, "Both", "Both"));
	addEnumSetting("Mode", "Mode", "What to show", mode);

	std::get<TextValue>(this->prefix).str = "CPS: ";
}

std::wstringstream CPSCounter::text(bool isDefault, bool inEditor) {
	std::wstringstream wss;
	switch (mode.getSelectedKey()) {
	case 0:
		wss << Latite::get().getTimings().getCPSL();
		break;
	case 1:
		wss << Latite::get().getTimings().getCPSR();
		break;
	case 2:
		wss << Latite::get().getTimings().getCPSL() << " | " << Latite::get().getTimings().getCPSR();
		break;
	}
	return wss;
}

