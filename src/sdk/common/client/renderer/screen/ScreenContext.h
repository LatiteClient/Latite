#pragma once
#include "util/ChronosMath.h"
#include "sdk/SDKBase.h"

namespace SDK {
	class ScreenContext : public Incomplete {
	public:
		CLASS_FIELD(Color*, shaderColor, 0x30);
		CLASS_FIELD(class Tessellator*, tess, 0xC0);
	};
}
