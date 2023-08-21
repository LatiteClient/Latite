#pragma once
#include "util/Util.h"

namespace SDK {

	// I'm not sure about this
	enum class Primitive {
		Quad = 1,
		TriangleFan,
		Trianglestrip,
		Linestrip
	};

	class Tessellator {
	public:
		void vertex(float x, float y, float z = 0.f);
		void begin(Primitive fmt, int numVertices);
		void color(float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
		void color(Color const& col);
	};
}