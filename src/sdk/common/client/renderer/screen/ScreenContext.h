#pragma once
#include "sdk/Util.h"

namespace SDK {
	class ScreenContext {
	public:
		CLASS_FIELD(Color*, shaderColor, 0x30);
		CLASS_FIELD(class Tessellator*, tess, 0xC0);
	};
}
