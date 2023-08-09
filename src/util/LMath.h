#pragma once
#ifdef RGB
#undef RGB
#endif

#include <cmath>

struct Vec2 final {
	float x, y;

	constexpr Vec2() : x(0.f), y(0.f) {}
	constexpr Vec2(float x, float y) : x(x), y(y) {}

	constexpr Vec2 operator-(Vec2 const& right) {
		return { x - right.x, y - right.y };
	}

	constexpr Vec2 operator+(Vec2 const& right) {
		return { x + right.x, y + right.y };
	}

	[[nodiscard]] float magnitude() {
		return static_cast<float>(std::sqrt(x * x + y * y));
	}
};

struct Vec3 final {
	float x, y, z;

	constexpr Vec3() : x(0.f), y(0.f), z(0.f) {}
	constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	//constexpr Vec3(struct Vec3i const& vec);
};

struct Vec3i final {
	int x, y, z;

	constexpr Vec3i() : x(0), y(0), z(0) {}
	constexpr Vec3i(int x, int y, int z) : x(x), y(y), z(z) {}
	constexpr Vec3i(Vec3 const& vec) : x(static_cast<int>(vec.x)), y(static_cast<int>(vec.y)), z(static_cast<int>(vec.z)) {}
};

//constexpr Vec3::Vec3(Vec3i const& vec) : x(static_cast<int>(std::round(vec.x))), y(static_cast<int>(std::round(vec.y))), z(static_cast<int>(std::round(vec.z))) {}

using BlockPos = Vec3i;

struct AABB final {
	Vec3 lower, higher;

	constexpr AABB(Vec3 lower, Vec3 higher) : lower(lower), higher(higher) {}
};

struct Color {
	float r, g, b, a;

	constexpr Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
	constexpr Color() : r(0.f), g(0.f), b(0.f), a(1.f) {}
	Color(float pFloat[4]) : r(pFloat[0]), g(pFloat[1]), b(pFloat[2]), a(pFloat[3]) {

	}
	
	static constexpr Color RGB(int r, int g, int b, int a = 255) { return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f); }
};

struct HSV {
	float h;
	float s;
	float v;

	constexpr HSV() : h(0.f), s(0.f), v(0.f) {}
	constexpr HSV(float h, float s, float v) : h(h), s(s), v(v) {}
};