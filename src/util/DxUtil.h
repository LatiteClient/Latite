#pragma once
#include <d2d1.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <d3d11_4.h>

#include <d2d1.h>
#include <d2d1_3.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <stdexcept>
#include <string>

#include <wrl/client.h>
#include <cmath>

#include "LMath.h"

#include "api/feature/setting/Setting.h"

#ifdef RGB
#undef RGB
#endif

using Microsoft::WRL::ComPtr;

template <class T> void SafeRelease(T** ppT) {
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}
using OColor = Color;

namespace d2d {

	class Color : public ::Color {
	public:
		constexpr Color(float r, float g, float b, float a = 1.f) : ::Color(r, g, b, a) {};

		constexpr Color() : ::Color(0.f, 0.f, 0.f, 1.f) {};

		constexpr Color(D2D1_COLOR_F col) : ::Color(col.r, col.g, col.b, col.a) {}

		constexpr Color(StoredColor const& col) : ::Color(col.r, col.g, col.b, col.a) {}

		Color(D2D1::ColorF::Enum e) : ::Color(0.f, 0.f, 0.f, 0.f) {
			D2D1::ColorF col = D2D1::ColorF(e);
			a = col.a;
			r = col.r;
			g = col.g;
			b = col.b;
		}

		constexpr Color(::Color c) : ::Color(c.r, c.g, c.b, c.a) {};

		constexpr Color operator+(Color right) {
			return Color(r + right.r, g + right.g, b + right.b, a);
		}

		constexpr Color operator+(float right) {
			return Color(r + right, g + right, b + right, a);
		}

		constexpr Color operator*(Color right) {
			return Color(r * right.r, g * right.g, b * right.b, a);
		}

		constexpr Color operator*(float right) {
			return Color(r * right, g * right, b * right, a * right);
		}

		constexpr Color operator-(Color right) {
			return Color(r - right.r, g - right.g, b - right.b, a);
		}

		constexpr Color operator-(float right) {
			return Color(r - right, g - right, b - right, a);
		}

		[[nodiscard]] bool isInvalid() const {
			return r < 0.f || g < 0.f || b < 0.f || isnan(r) || isnan(g) || isnan(b) || isinf(r) || isinf(g) || isinf(b);
		}

		[[nodiscard]] static constexpr Color RGB(int r, int g, int b, int alpha = 255) {
			return Color((float)r / 255.f, (float)g / 255.f, (float)b / 255.f, (float)alpha / 255.f);
		};

		[[nodiscard]] static Color Hex(std::string const& str, float alpha = 1.f) {
			Color myColor(0.f, 0.f, 0.f, 0.f);
			myColor.a = alpha;

			std::string buffer = str.substr(1);

			std::string rBuf = buffer.substr(0, 2);
			myColor.r = static_cast<float>(std::stoi(rBuf, nullptr, 16)) / 255.f;

			std::string gBuf = buffer.substr(2, 2);
			myColor.g = static_cast<float>(std::stoi(gBuf, nullptr, 16)) / 255.f;

			std::string bBuf = buffer.substr(4, 2);
			myColor.b = static_cast<float>(std::stoi(bBuf, nullptr, 16)) / 255.f;
			return myColor;
		}

		[[nodiscard]] std::string getHex() const;

		[[nodiscard]] constexpr Color asAlpha(float al) const {
			return { r, g, b, al };
		}

		[[nodiscard]] constexpr D2D1_COLOR_F get() const {
			return D2D1_COLOR_F(r, g, b, a);
		}
	};

	namespace Colors {
		inline static Color WHITE  =  { 1.f, 1.f, 1.f, 1.f };
		inline static Color RED    =  { 1.f, 0.f, 0.f, 1.f };
		inline static Color GREEN  =  { 0.f, 1.f, 0.f, 1.f };
		inline static Color BLUE   =  { 0.f, 0.f, 1.f, 1.f };
		inline static Color YELLOW =  { 1.F, 1.F, 0.f, 1.f };
		inline static Color AQUA   =  { 0.f, 1.F, 1.f, 1.f };
		inline static Color PURPLE =  { 1.f, 0.f, 1.f, 1.f };
		inline static Color BLACK  =  { 0.f, 0.f, 0.f, 1.f };
	}

	class Rect {
	public:
		Rect(Vec2 const& p1, Vec2 const& p2) : left(p1.x), top(p1.y), right(p2.x), bottom(p2.y) {}
		Rect(float left, float top, float right, float bottom) : left(left), top(top), right(right), bottom(bottom) {}
		Rect() : left(0.f), top(0.f), right(0.f), bottom(0.f) {}

		float left, top, right, bottom;

		[[nodiscard]] D2D1_RECT_F get() const {
			return D2D1::RectF(left, top, right, bottom);
		}

		[[nodiscard]] Vec2 getPos() const {
			return { left, top };
		}

		void setPos(Vec2 const& pos) {
			float bOffX = right - left;
			float bOffY = bottom - top;

			left = pos.x;
			top = pos.y;

			right = left + bOffX;
			bottom = top + bOffY;
		}

		[[nodiscard]] Vec2 getSize() const {
			return { right - left, bottom - top };
		}

		[[nodiscard]] float getWidth() const {
			return right - left;
		}

		[[nodiscard]] float getHeight() const {
			return bottom - top;
		}

		void round() {
			left = std::round(left);
			top = std::round(top);
			right = std::round(right);
			bottom = std::round(bottom);
		}

		[[nodiscard]] bool contains(Vec2 pt) const {
			return pt.x >= left && pt.x <= right && pt.y <= bottom && pt.y >= top;
		}

		[[nodiscard]] bool contains(D2D1_RECT_F other) const {
			Vec2 pt1 = { other.left, other.top };
			Vec2 pt2 = { other.right, other.bottom };

			return contains(pt1) && contains(pt2);
		}

		[[nodiscard]] Vec2 center(Vec2 otherSize = { 0, 0 }) const {
			return Vec2(left + ((right - left) / 2 - (otherSize.x / 2)), top + ((bottom - top) / 2 - (otherSize.y / 2)));
		}

		[[nodiscard]] float centerY(float otherSize = 0.f) const {
			return top + ((bottom - top) / 2) - (otherSize / 2);
		}

		[[nodiscard]] float centerX(float otherSize = 0.f) const {
			return left + ((right - left) / 2) - (otherSize / 2);
		}

		// Center Y-axis, X is left
		[[nodiscard]] Vec2 centerYL() const {
			return Vec2(left, top + ((bottom - top) / 2));
		}

		// Center Y-axis, X is right
		[[nodiscard]] Vec2 centerYR() const {
			return Vec2(right, top + ((bottom - top) / 2));
		}

		// Center X-axis, Y is top
		[[nodiscard]] Vec2 centerXT() const {
			return Vec2(left + ((right - left) / 2), top);
		}

		// Center X-axis, Y is bottom
		[[nodiscard]] Vec2 centerXB() const {
			return Vec2(left + ((right - left) / 2), bottom);
		}

		[[nodiscard]] d2d::Rect translate(Vec2 const& vec) {
			return Rect(left + vec.x, top + vec.y, right + vec.x, bottom + vec.y);
		}

		[[nodiscard]] d2d::Rect translate(const float x, const float y) {
			return Rect(left + x, top + y, right + x, bottom + y);
		}

		operator D2D1_RECT_F() {
			return { left, top, right, bottom };
		}
	};
}

namespace util {
	void doThrowIfFailed(HRESULT hr, int line, std::string func);
}

#define ThrowIfFailed(...) util::doThrowIfFailed(__VA_ARGS__, __LINE__, __func__)