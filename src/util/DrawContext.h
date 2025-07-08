#pragma once
#include "LMath.h"
#include "DxUtil.h"
#include "client/render/Renderer.h"
#include "mc/common/client/renderer/screen/MinecraftUIRenderContext.h"
#include "mc/common/client/gui/Font.h"

// Base DrawUtil class (provides no drawing implementation).
class DrawUtil {
protected:
	bool immediate = true;
public:
	enum class RenderType {
		D2D,
		Game
	} renderType = RenderType::D2D;

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
	virtual void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true, bool hyphen = false) = 0;
	/// Be careful when setting cache to true
	virtual Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true, std::optional<Vec2> bounds = std::nullopt) = 0;
	/// Be careful when setting cache to true
	virtual RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true) = 0;

	/// Flushes the content to the screen.
	virtual void flush(bool text = true, bool mesh = true) {}

	/// Sets whether the content will be immediately flushed to the screen (if supported)
	void setImmediate(bool b) { immediate = b; }

	virtual bool isMinecraft() { return false; }

	DrawUtil() = default;
	virtual ~DrawUtil() = default;
};

// Direct2D implementation for the DrawUtil class. Also includes D2D specific functions.
class D2DUtil final : public DrawUtil {
public:
	D2DUtil();
	~D2DUtil() = default;

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
	virtual void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true, bool hyphen = false) override;
	virtual Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true, std::optional<Vec2> bounds = std::nullopt) override;
	virtual RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true) override;
};

// Minecraft Renderer implementation of DrawUtil class (couldn't think of a good name)
class MCDrawUtil final : public DrawUtil {
public:
	SDK::RectangleArea getRect(d2d::Rect const& rc);
	bool isMinecraft() override { return true; }

	// Game Draw members
	SDK::MinecraftUIRenderContext* renderCtx;
	SDK::ScreenContext* scn;
	SDK::Font* font;
	float guiScale = 1.f;

	MCDrawUtil(SDK::MinecraftUIRenderContext* ctx, SDK::Font* font) : renderCtx(ctx), scn(renderCtx->screenContext), font(font), guiScale(SDK::ClientInstance::get()->getGuiData()->guiScaleFrac) {}
	void flush(bool text = true, bool mesh = true) override;

	void drawVignette(d2d::Color const& innerCol, float fade = 0.f);
	void drawImage(SDK::TexturePtr& texture, Vec2 const& pos, Vec2 const& size, d2d::Color const& flushCol);
	d2d::Rect drawItem(SDK::ItemStack* item, Vec2 const& pos, float sizeModifier = 1.f, float opacity = 1.f);

	void fillPolygon(Vec2 const& center, float radius, int numSides, d2d::Color const& col);
	void drawPolygon(Vec2 const& center, float radius, int numSides, d2d::Color const& col, float lineThickness = 1.f);
	virtual void fillRectangle(RectF const& rect, d2d::Color const& color) override;
	virtual void drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness = 1.f) override;
	virtual void fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius = 10.f) override;
	virtual void drawRoundedRectangle(RectF, d2d::Color const& color, float radius = 10.f, float lineThickness = 1.f, OutlinePosition outPos = OutlinePosition::Center) override;
	virtual void drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size = 30.f, DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool cache = true, bool hyphen = false) override;
	virtual Vec2 getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace = true, bool cache = true, std::optional<Vec2> bounds = std::nullopt) override;
	virtual RectF getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad = 0.f, bool cache = true) override;
};