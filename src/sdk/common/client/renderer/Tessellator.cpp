#include "pch.h"
#include "Tessellator.h"
/*#include "mnemosyne/Scanner.h"

void SDK::Tessellator::vertex(float x, float y, float z) {
	static auto func = util::InstructionToAddress(mnem::ScanSignature("e8 ? ? ? ? f3 0f 10 5b ? f3 0f 10 13"_sig), 1);
	reinterpret_cast<void(*)(void*, float, float, float)>(func)(this, x, y, z);
}

void SDK::Tessellator::begin(Primitive fmt, int numVertices) {
	static auto func = mnem::ScanSignature("48 89 5C 24 ?? 55 48 83 EC ?? 80 B9 ?? ?? ?? ?? 00 45"_sig);
	reinterpret_cast<void(*)(void*, Primitive, int)>(func)(this, (Primitive)fmt, numVertices);
}

void SDK::Tessellator::color(float r, float g, float b, float a) {
	static auto func = mnem::ScanSignature("80 b9 ? ? ? ? ? 4c 8b c1 75"_sig);
	reinterpret_cast<void(*)(void*, float, float, float, float)>(func)(this, r, g, b, a);
}

void SDK::Tessellator::color(Color const& col) {
	color(col.r, col.g, col.b, col.a);
}
*/