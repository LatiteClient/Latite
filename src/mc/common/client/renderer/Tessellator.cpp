#include "pch.h"
#include "Tessellator.h"

void SDK::Tessellator::vertex(float x, float y, float z) {
	if (!Signatures::Tessellator_vertex.result) return;
	reinterpret_cast<void(*)(Tessellator*, float, float, float)>(Signatures::Tessellator_vertex.result)(this, x, y, z);
}

void SDK::Tessellator::begin(Primitive fmt, int numVertices) {
	if (!Signatures::Tessellator_begin.result) return;
	reinterpret_cast<void(*)(Tessellator*, int, Primitive, int, bool)>(Signatures::Tessellator_begin.result)(this, 0, fmt, numVertices, false);
}

void SDK::Tessellator::color(float r, float g, float b, float a) {
	if (!Signatures::Tessellator_color.result) return;
	reinterpret_cast<void(*)(Tessellator*, const Color&)>(Signatures::Tessellator_color.result)(this, Color{r, g, b, a});
}

void SDK::Tessellator::color(Color const& col) {
	color(col.r, col.g, col.b, col.a);
}
