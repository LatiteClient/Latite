#pragma once
#include "sdk/Util.h"
#include "util/LMath.h"

namespace SDK {
	class GuiData {
	public:
		MVCLASS_FIELD(Vec2, screenSize, 0x30, 0x18, 0x30);
		MVCLASS_FIELD(Vec2, guiSize, 0x40, 0x28, 0x40);
		MVCLASS_FIELD(float, guiScale, 0x4C, 0x34, 0x4C);
		MVCLASS_FIELD(float, guiScaleFrac, 0x50, 0x38, 0x50);

		void updateGuiScale(float newScale) {
			guiScale = newScale;
			guiScaleFrac = 1.f / newScale;
			guiSize = Vec2((screenSize.x * guiScaleFrac), screenSize.y * guiScaleFrac);
		}
	};
}