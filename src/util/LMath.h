#pragma once
#ifdef RGB
#undef RGB
#endif

struct Vec2 final {
	float x, y;

	constexpr Vec2(float x, float y) : x(x), y(y) {}
};

struct Vec3 final {
	float x, y, z;

	constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct AABB final {
	Vec3 lower, higher;

	constexpr AABB(Vec3 lower, Vec3 higher) : lower(lower), higher(higher) {}
};

struct Color {
	float r, g, b, a;

	constexpr Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
	constexpr Color() : r(0.f), g(0.f), b(0.f), a(1.f) {}
	
	static constexpr Color RGB(int r, int g, int b, int a = 255) { return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f); }
};