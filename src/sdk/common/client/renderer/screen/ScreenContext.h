#pragma once
#include "sdk/Util.h"
#include <stack>

namespace SDK {
	class ScreenContext {
	public:
		CLASS_FIELD(Color*, shaderColor, 0x30);
		CLASS_FIELD(class Tessellator*, tess, 0xC8);
		
		struct MatrixPtr {
			MVCLASS_FIELD(std::stack<D2D1::Matrix4x4F>, matrixStack, 0x40, 0x30, 0x30);
		};
		CLASS_FIELD(MatrixPtr*, matrix, 0x18);

		inline static ScreenContext* instance3d = nullptr;
	};
}
