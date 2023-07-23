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
	void fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius = 10.f);
	void drawRoundedRectangle(RectF, d2d::Color const& color, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center);
	void drawGaussianBlur(float intensity = 5.f);
	void setFont(Renderer::FontSelection font);
	void drawText(RectF const& rc, std::string const& text, d2d::Color const& color, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_TRAILING);

	DXContext();
	~DXContext() = default;
};
