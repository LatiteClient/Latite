#pragma once
#include "mc/Util.h"
#include <stack>

namespace SDK {
	class ScreenContext {
	public:
		CLASS_FIELD(Color*, shaderColor, 0x30);
		CLASS_FIELD(class Tessellator*, tess, 0xC8);

		// this is mce::Camera
		struct MatrixPtr {
			MVCLASS_FIELD(std::stack<D2D1::Matrix4x4F>, viewMatrixStack, 0x0, 0x0, 0x0);
			// worldMatrixStack
			MVCLASS_FIELD(std::stack<D2D1::Matrix4x4F>, matrixStack, 0x40, 0x30, 0x30);
			MVCLASS_FIELD(std::stack<D2D1::Matrix4x4F>, projectionMatrixStack, 0x80, 0x60, 0x60);
		};
		CLASS_FIELD(MatrixPtr*, matrix, 0x18);

		inline static ScreenContext* instance3d = nullptr;
	};
}
