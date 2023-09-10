#pragma once
#include "LMath.h"
#include "DxUtil.h"
#include "client/render/Renderer.h"
#include "sdk/common/client/renderer/screen/MinecraftUIRenderContext.h"
#include "sdk/common/client/gui/Font.h"

// Base DrawUtil class (provides no drawing implementation).
class DrawUtil {
public:
	enum class OutlinePosition : int {
		Center,
		Inside,
		Outside
	};

	using RectF = d2d::Rect;

	virtual void fillRectangle(RectF const& rect, d2d::Color const& color) = 0;
	virtual void drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness = 1.f) = 0;

	virtual void fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius = 10.f) = 0;
	virtual void drawRoundedRectangle(RectF, d2d::Color const& color, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center) = 0;

	/// Draws text. Be careful when setting cache to true
	virtual void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true) = 0;
	/// Be careful when setting cache to true
	virtual Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true) = 0;
	/// Be careful when setting cache to true
	virtual RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true) = 0;

	DrawUtil() = default;
	virtual ~DrawUtil() = default;
};

// Direct2D implementation for the DrawUtil class. Also includes D2D specific functions.
class D2DUtil : public DrawUtil {
public:
	// D2D/DWrite members
	D2D1_RECT_F	getRect(RectF const& rc);

	ID2D1DeviceContext* ctx = nullptr;
	ID2D1SolidColorBrush* brush = nullptr;
	IDWriteTextFormat* currentFormat = nullptr;
	IDWriteFactory* factory = nullptr;

	// D2D/DWrite-specific functions

	virtual void drawGaussianBlur(float intensity = 5.f);
	virtual void drawGaussianBlur(ID2D1Bitmap1* bmp, float intensity = 5.f);

	virtual void fillRectangle(RectF const& rect, ID2D1Brush* cbrush);
	virtual void drawRectangle(RectF const& rect, ID2D1Brush* cbrush, float lineThickness = 1.f);

	virtual void drawRoundedRectangle(RectF, ID2D1Brush*, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center);
	virtual void fillRoundedRectangle(RectF const& rect, ID2D1Brush* cbrush, float radius = 10.f);

	// Inherited from DrawUtil

	virtual void fillRectangle(RectF const& rect, d2d::Color const& color) override;
	virtual void drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness = 1.f) override;
	virtual void fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius = 10.f) override;
	virtual void drawRoundedRectangle(RectF, d2d::Color const& color, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center) override;
	virtual void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true) override;
	virtual Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true) override;
	virtual RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true) override;
};

// Minecraft Renderer implementation of DrawUtil class (couldn't think of a good name)
class MCDrawUtil : public DrawUtil {
public:
	SDK::RectangleArea getRect(d2d::Rect const& rc);

	// Game Draw members
	SDK::MinecraftUIRenderContext* renderCtx;
	SDK::ScreenContext* scn;
	SDK::Font* font;
	float guiScale = 1.f;

	MCDrawUtil(SDK::MinecraftUIRenderContext* ctx, SDK::Font* font) : renderCtx(ctx), scn(renderCtx->screenContext), font(font), guiScale(SDK::ClientInstance::get()->getGuiData()->guiScaleFrac) {}

	virtual void fillRectangle(RectF const& rect, d2d::Color const& color) override;
	virtual void drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness = 1.f) override;
	virtual void fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius = 10.f) override;
	virtual void drawRoundedRectangle(RectF, d2d::Color const& color, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center) override;
	virtual void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true) override;
	virtual Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true) override;
	virtual RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true) override;
};