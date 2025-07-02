#pragma once
#ifdef RGB
#undef RGB
#endif

#include <cmath>

static constexpr float pi_f = 3.1415926535f;

namespace LatiteMath {
	static constexpr float abs(float f) {
		return f < 0.f ? -f : f;
	}

	static constexpr float deg2rad(float deg) {
		return deg * (pi_f / 180.f);
	}

	static constexpr bool aequals(float a, float b) {
		return abs(b - a) < 0.01f;
	}
}

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
	
	constexpr Vec2 operator/(Vec2 const& right) {
		return { x / right.x, y / right.y };
	}

	constexpr Vec2 operator*(Vec2 const& right) {
		return { x * right.x, y * right.y };
	}

	bool operator==(Vec2 const& right) const {
		return x == right.x && y == right.y;
	}

	bool operator!=(Vec2 const& right) const {
		return x != right.x || y != right.y;
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

	constexpr Vec3 operator-(Vec3 const& right) const {
		return { x - right.x, y - right.y, z - right.z };
	}

	constexpr Vec3 operator+(Vec3 const& right) const {
		return { right.x + x, right.y + y, right.z + z };
	}

	constexpr Vec3 operator*(float right) const {
		return { x * right, y * right, z * right };
	}

	bool operator==(Vec3 const& right) const {
		return x == right.x && y == right.y && z == right.z;
	}

	bool operator!=(Vec3 const& right) const {
		return x != right.x || y != right.y || z != right.z;
	}

	inline float distance(Vec3& vec) {
		return static_cast<float>(std::sqrt(std::pow(x - vec.x, 2) + std::pow(y - vec.y, 2) + std::pow(z - vec.z, 2)));
	}
};

struct Vec4 final {
	float x, y, z, w;

	constexpr Vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct Vec3i final {
	int x, y, z;

	constexpr Vec3i() : x(0), y(0), z(0) {}
	constexpr Vec3i(int x, int y, int z) : x(x), y(y), z(z) {}
	constexpr Vec3i(Vec3 const& vec) : x(static_cast<int>(vec.x)), y(static_cast<int>(vec.y)), z(static_cast<int>(vec.z)) {}
};

struct Vec3Hasher final {
	std::size_t operator()(const Vec3& v) const {
		std::hash<float> hasher;
		size_t hx = hasher(v.x);
		size_t hy = hasher(v.y);
		size_t hz = hasher(v.z);
		return hx ^ (hy << 1) ^ (hz << 2);
	}
};

//constexpr Vec3::Vec3(Vec3i const& vec) : x(static_cast<int>(std::round(vec.x))), y(static_cast<int>(std::round(vec.y))), z(static_cast<int>(std::round(vec.z))) {}

using BlockPos = Vec3i;

struct AABB final {
	Vec3 lower, higher;

	constexpr AABB(Vec3 lower, Vec3 higher) : lower(lower), higher(higher) {}

	Vec3 getCenter() {
		return (lower + higher) * 0.5f;
	}

	void rebase(Vec3 newV) {
		Vec3 cent = getCenter();

		Vec3 diff = higher - cent;

		this->lower = newV - diff;
		this->higher = newV + diff;
	}

	Vec3 closestPoint(Vec3 const& to) {
		return {
			std::clamp(to.x, lower.x, higher.x),
			std::clamp(to.y, lower.y, higher.y),
			std::clamp(to.z, lower.z, higher.z),
		};
	}
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
