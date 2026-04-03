#pragma once
#include "mc/Util.h"
#include "util/LMath.h"

namespace SDK {
	class GuiData {
	public:
		CLASS_FIELD(Vec2, screenSize, 0x40);
		CLASS_FIELD(Vec2, guiSize, 0x50);
		CLASS_FIELD(float, guiScale, 0x5C);
		CLASS_FIELD(float, guiScaleFrac, 0x60);

		void updateGuiScale(float newScale) {
			guiScale = newScale;
			guiScaleFrac = 1.f / newScale;
			guiSize = Vec2((screenSize.x * guiScaleFrac), screenSize.y * guiScaleFrac);
		}

		void displayClientMessage(const std::string& message) {
			static auto sig = Signatures::GuiData_displayClientMessage.result;
			using func_t = void(*)(GuiData*, const std::string&, std::optional<std::string>, bool);
			static auto func = reinterpret_cast<func_t>(sig);
			func(this, message, {}, false);
		}
	};
}