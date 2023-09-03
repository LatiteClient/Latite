#pragma once
#include "LMath.h"
#include "DxUtil.h"
#include "client/render/Renderer.h"

class DXContext {
public:
	ID2D1DeviceContext* ctx = nullptr;
	ID2D1SolidColorBrush* brush = nullptr;
	IDWriteTextFormat* currentFormat = nullptr;
	IDWriteFactory* factory = nullptr;
private:
public:
	enum class OutlinePosition : int {
		Center,
		Inside,
		Outside
	};

	using RectF = d2d::Rect;

	D2D1_RECT_F	getRect(RectF const& rc);
	void fillRectangle(RectF const& rect, d2d::Color const& color);
	void drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness = 1.f);

	void fillRectangle(RectF const& rect, ID2D1Brush* cbrush);
	void drawRectangle(RectF const& rect, ID2D1Brush* cbrush, float lineThickness = 1.f);

	void fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius = 10.f);
	void drawRoundedRectangle(RectF, d2d::Color const& color, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center);
	
	void drawRoundedRectangle(RectF, ID2D1Brush*, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center);
	void fillRoundedRectangle(RectF const& rect, ID2D1Brush* cbrush, float radius = 10.f);

	void drawGaussianBlur(float intensity = 5.f);
	// More optimized (please use this)
	void drawGaussianBlur(ID2D1Bitmap1* bmp, float intensity = 5.f);

	/// Draws text. Be careful when setting cache to true
	void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true);
	/// Be careful when setting cache to true
	Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true);
	/// Be careful when setting cache to true
	RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true);

	DXContext();
	~DXContext() = default;
};
