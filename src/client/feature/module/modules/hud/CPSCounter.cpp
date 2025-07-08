#include "pch.h"
#include "CPSCounter.h"
#include "client/Latite.h"

CPSCounter::CPSCounter() : TextModule("CPS", LocalizeString::get("client.textmodule.cpsCounter.name"),
                                      LocalizeString::get("client.textmodule.cpsCounter.desc"), HUD, 400.f, 0,
                                      true /*cache CPS text*/) {
    mode.addEntry(EnumEntry(0, LocalizeString::get("client.textmodule.cpsCounter.modeState0.name"),
                            LocalizeString::get("client.textmodule.cpsCounter.modeState0.name")));
    mode.addEntry(EnumEntry(1, LocalizeString::get("client.textmodule.cpsCounter.modeState1.name"),
                            LocalizeString::get("client.textmodule.cpsCounter.modeState1.name")));
    mode.addEntry(EnumEntry(2, LocalizeString::get("client.textmodule.cpsCounter.modeState2.name"),
                            LocalizeString::get("client.textmodule.cpsCounter.modeState2.name")));
    addEnumSetting("Mode", LocalizeString::get("client.textmodule.cpsCounter.mode.name"),
                   LocalizeString::get("client.textmodule.cpsCounter.mode.desc"), mode);

    std::get<TextValue>(this->prefix).str = L"CPS: ";
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

