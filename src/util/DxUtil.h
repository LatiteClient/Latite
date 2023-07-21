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

#ifdef RGB
#undef RGB
#endif

using Microsoft::WRL::ComPtr;

template <class T> void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

namespace d2d {
	class Color {
	public:
		float r, g, b, a;

		Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {};

		Color() : r(0.f), g(0.f), b(0.f), a(1.f) {};

		Color(D2D1::ColorF col) : r(col.r), g(col.g), b(col.b), a(col.a) {}

		Color(D2D1::ColorF::Enum e) {
			D2D1::ColorF col = D2D1::ColorF(e);
			a = col.a;
			r = col.r;
			g = col.g;
			b = col.b;
		}

		Color operator+(Color right) {
			return Color(r + right.r, g + right.g, b + right.b, a);
		}

		Color operator+(float right) {
			return Color(r + right, g + right, b + right, a);
		}

		Color operator*(Color right) {
			return Color(r * right.r, g * right.g, b * right.b, a);
		}

		Color operator*(float right) {
			return Color(r * right, g * right, b * right, a * right);
		}

		Color operator-(Color right) {
			return Color(r - right.r, g - right.g, b - right.b, a);
		}

		Color operator-(float right) {
			return Color(r - right, g - right, b - right, a);
		}

		bool isInvalid() const {
			return r < 0.f || g < 0.f || b < 0.f || isnan(r) || isnan(g) || isnan(b) || isinf(r) || isinf(g) || isinf(b);
		}

		static Color RGB(int r, int g, int b, int alpha = 255) {
			return Color((float)r / 255.f, (float)g / 255.f, (float)b / 255.f, (float)alpha / 255.f);
		};

		static Color Hex(std::string const& str, float alpha = 1.f) {
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

		std::string getHex() const;

		void rr(int a) {
			r = (float)a / 255.f;
		}

		void gg(int a) {
			g = (float)a / 255.f;
		}

		void bb(int a) {
			b = (float)a / 255.f;
		}

		Color asAlpha(float al) const {
			return { r, g, b, al };
		}

		D2D1_COLOR_F get() const {
			return D2D1_COLOR_F(r, g, b, a);
		}
	};
}

namespace util {
	void doThrowIfFailed(HRESULT hr, int line, std::string func);
}

#define ThrowIfFailed(...) util::doThrowIfFailed(__VA_ARGS__, __LINE__, __func__)