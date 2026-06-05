#pragma once
#include "mc/Util.h"
#include <glm/mat4x4.hpp>
#include <stack>

namespace SDK {
	class ScreenContext {
	public:
		CLASS_FIELD(Color*, shaderColor, 0x30);
		CLASS_FIELD(class Tessellator*, tess, 0xB8);

		// this is mce::Camera
		struct MatrixPtr {
			CLASS_FIELD(std::stack<glm::mat4>, viewMatrixStack, 0x0);
			// worldMatrixStack
			CLASS_FIELD(std::stack<glm::mat4>, matrixStack, 0x40);
			CLASS_FIELD(std::stack<glm::mat4>, projectionMatrixStack, 0x80);
		};
		CLASS_FIELD(MatrixPtr*, matrix, 0x18);

		inline static ScreenContext* instance3d = nullptr;
	};
}
