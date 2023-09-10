#include "pch.h"
#include "DxContext.h"
#include "client/Latite.h"
#include "Util.h"
#include "sdk/common/client/renderer/Tessellator.h"
#include "sdk/common/client/renderer/MeshUtils.h"
#include "sdk/common/client/renderer/MaterialPtr.h"

D2D1_RECT_F D2DUtil::getRect(RectF const& rc)  {
	return D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom);
}

void D2DUtil::fillRectangle(RectF const& rect, d2d::Color const& color)  {
	this->brush->SetColor(color.get());
	ctx->FillRectangle(getRect(rect), brush);
}

void D2DUtil::drawRectangle(RectF const& rect, d2d::Color const& color, float lineThickness)  {
	this->brush->SetColor(color.get());
	ctx->DrawRectangle(getRect(rect), brush, lineThickness);
}

void D2DUtil::fillRectangle(RectF const& rect, ID2D1Brush* cbrush) {
	ctx->FillRectangle(getRect(rect), cbrush);
}

void D2DUtil::drawRectangle(RectF const& rect, ID2D1Brush* cbrush, float lineThickness) {
	ctx->DrawRectangle(getRect(rect), cbrush, lineThickness);
}

void D2DUtil::fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius)  {
	this->brush->SetColor(color.get());
	if (radius < 0.005f) {
		ctx->FillRectangle(rect.get(), brush);
		return;
	}

	auto rounded = D2D1::RoundedRect(getRect(rect), radius, radius);
	ctx->FillRoundedRectangle(rounded, brush);
}

void D2DUtil::drawRoundedRectangle(RectF irect, d2d::Color const& color, float radius, float lineThickness, OutlinePosition outPos)  {
	this->brush->SetColor(color.get());
	if (radius < 0.005f) {
		ctx->DrawRectangle(irect, brush, lineThickness);
		return;
	}
	
	RectF rect = irect;
	switch (outPos) {
	case OutlinePosition::Inside:
		rect.left += (lineThickness / 2.f);
		rect.right -= (lineThickness / 2.f);
		rect.top += (lineThickness / 2.f);
		rect.bottom -= (lineThickness / 2.f);
		break;
	case OutlinePosition::Outside:
		rect.left -= lineThickness / 2.f;
		rect.right += lineThickness / 2.f;
		rect.top -= lineThickness / 2.f;
		rect.bottom += lineThickness / 2.f;
		radius += lineThickness / 2.f;
		break;
	default:
		break;
	}
	auto rc = getRect(rect);
	auto rounded = D2D1::RoundedRect(rc, radius, radius);

	ctx->DrawRoundedRectangle(rounded, brush, lineThickness);
}

void D2DUtil::drawRoundedRectangle(RectF irect, ID2D1Brush* cbrush, float radius, float lineThickness, OutlinePosition outPos)
{
	RectF rect = irect;
	switch (outPos) {
	case OutlinePosition::Inside:
		rect.left += (lineThickness / 2.f);
		rect.right -= (lineThickness / 2.f);
		rect.top += (lineThickness / 2.f);
		rect.bottom -= (lineThickness / 2.f);
		break;
	case OutlinePosition::Outside:
		rect.left -= lineThickness / 2.f;
		rect.right += lineThickness / 2.f;
		rect.top -= lineThickness / 2.f;
		rect.bottom += lineThickness / 2.f;
		radius += lineThickness / 2.f;
		break;
	default:
		break;
	}
	auto rc = getRect(rect);
	auto rounded = D2D1::RoundedRect(rc, radius, radius);

	ctx->DrawRoundedRectangle(rounded, cbrush, lineThickness);
}

void D2DUtil::fillRoundedRectangle(RectF const& rect, ID2D1Brush* cbrush, float radius)
{
	auto rc = getRect(rect);
	auto rounded = D2D1::RoundedRect(rc, radius, radius);

	ctx->FillRoundedRectangle(rounded, cbrush);
}

void D2DUtil::drawGaussianBlur(float intensity)  {
	ID2D1Effect* gaussianBlurEffect = Latite::getRenderer().getBlurEffect();
	
	// maybe we might not need to flush if we dont draw anything before clickgui?
	ctx->Flush();
	auto bitmap = Latite::getRenderer().getBlurBitmap();
	gaussianBlurEffect->SetInput(0, bitmap);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);
	auto sz = ctx->GetPixelSize();
	auto rc = D2D1::RectF(0, 0, (float)sz.width, (float)sz.height);
	D2D1::Matrix3x2F oMat;
	//ctx->SetTransform(D2D1::Matrix3x2F::Scale(sz.width / rc.right, sz.height / rc.bottom));
	ctx->DrawImage(gaussianBlurEffect, D2D1::Point2F(0.f, 0.f), rc);
}

void D2DUtil::drawGaussianBlur(ID2D1Bitmap1* bmp, float intensity) {
	ID2D1Effect* gaussianBlurEffect = Latite::getRenderer().getBlurEffect();

	ctx->Flush();
	Latite::getRenderer().copyCurrentBitmap(bmp);
	gaussianBlurEffect->SetInput(0, bmp);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, intensity);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
	gaussianBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);
	auto sz = ctx->GetPixelSize();
	auto rc = D2D1::RectF(0, 0, (float)sz.width, (float)sz.height);
	D2D1::Matrix3x2F oMat;
	//ctx->SetTransform(D2D1::Matrix3x2F::Scale(sz.width / rc.right, sz.height / rc.bottom));
	ctx->DrawImage(gaussianBlurEffect, D2D1::Point2F(0.f, 0.f), rc);
}

void D2DUtil::drawText(RectF const& rc, std::wstring const& ws, d2d::Color const& color, Renderer::FontSelection font, float size, DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT verticalAlignment, bool cache)  {
	ComPtr<IDWriteTextFormat> fmt = Latite::getRenderer().getTextFormat(font);
	brush->SetColor(color.get());
	if (auto layout = Latite::getRenderer().getLayout(fmt.Get(), ws, cache)) {
		layout->SetMaxWidth(rc.getWidth());
		layout->SetMaxHeight(rc.getHeight());
		DWRITE_TEXT_RANGE range{};
		range.startPosition = 0;
		range.length = static_cast<UINT32>(ws.size());
		layout->SetFontSize(size, range);
		layout->SetTextAlignment(alignment);
		layout->SetParagraphAlignment(verticalAlignment);
		this->ctx->DrawTextLayout({ rc.getPos().x, rc.getPos().y }, layout, brush);
	}
}

Vec2 D2DUtil::getTextSize(std::wstring const& ws, Renderer::FontSelection font, float size, bool tw, bool cache) {
	ComPtr<IDWriteTextFormat> fmt = Latite::getRenderer().getTextFormat(font);
	auto ss = ctx->GetPixelSize();
	if (auto layout = Latite::getRenderer().getLayout(fmt.Get(), ws, cache)) {
		layout->SetMaxWidth(static_cast<float>(ss.width));
		layout->SetMaxHeight(static_cast<float>(ss.height));
		DWRITE_TEXT_RANGE range;
		range.startPosition = 0;
		range.length = static_cast<UINT32>(ws.size());
		layout->SetFontSize(size, range);
		layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		DWRITE_TEXT_METRICS textMetrics;
		DWRITE_OVERHANG_METRICS overhangs;
		layout->GetMetrics(&textMetrics);
		layout->GetOverhangMetrics(&overhangs);
		
		float width = tw ? (textMetrics.widthIncludingTrailingWhitespace - overhangs.left) : ((textMetrics.layoutWidth) + overhangs.right) - (textMetrics.left - overhangs.left);
		float height = textMetrics.height;
		
		return Vec2(width, height);
	}
	return {};
}

d2d::Rect D2DUtil::getTextRect(std::wstring const& ws, Renderer::FontSelection font, float size, float pad, bool cache) {
	ComPtr<IDWriteTextFormat> fmt = Latite::getRenderer().getTextFormat(font);
	auto ss = ctx->GetPixelSize();
	ComPtr<IDWriteTextLayout> layout;
	if (auto layout = Latite::getRenderer().getLayout(fmt.Get(), ws, cache)) {
		layout->SetMaxWidth(static_cast<float>(ss.width));
		layout->SetMaxHeight(static_cast<float>(ss.height));
		DWRITE_TEXT_RANGE range;
		range.startPosition = 0;
		range.length = static_cast<UINT32>(ws.size());
		layout->SetFontSize(size, range);
		layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		layout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		DWRITE_TEXT_METRICS metrics;
		DWRITE_OVERHANG_METRICS overhangs;
		layout->GetMetrics(&metrics);
		layout->GetOverhangMetrics(&overhangs);

		return {
			metrics.left - overhangs.left - pad, metrics.top - overhangs.top - pad,
			metrics.layoutWidth + overhangs.right + pad, metrics.layoutHeight + overhangs.bottom + pad
		};
	}
	return {};
}

D2DUtil::D2DUtil() : brush(Latite::getRenderer().getSolidBrush()), ctx(Latite::getRenderer().getDeviceContext()), factory(Latite::getRenderer().getDWriteFactory())  {
}

SDK::RectangleArea MCDrawUtil::getRect(d2d::Rect const& rc) {
	return SDK::RectangleArea(rc.left * guiScale, rc.top * guiScale, rc.right * guiScale, rc.bottom * guiScale);
}

void MCDrawUtil::flush() {
	SDK::MeshHelpers::renderMeshImmediately(scn, scn->tess, SDK::MaterialPtr::getUIColor());
	renderCtx->flushText(0.f);
}

void MCDrawUtil::fillRectangle(RectF const& rc, d2d::Color const& color) {
	auto rect = getRect(rc);
	auto tess = scn->tess;
	tess->begin(SDK::Primitive::Quad, 4);
	*scn->shaderColor = { 1.f, 1.f, 1.f, 1.f };
	tess->color(color);

	scn->tess->vertex(rect.left, rect.bottom);
	scn->tess->vertex(rect.right, rect.bottom);
	scn->tess->vertex(rect.right, rect.top);
	scn->tess->vertex(rect.left, rect.top);

	if (immediate) SDK::MeshHelpers::renderMeshImmediately(scn, tess, SDK::MaterialPtr::getUIColor());
}

void MCDrawUtil::drawRectangle(RectF const& rect, d2d::Color const& col, float lineThickness) {
	bool oImm = immediate;
	immediate = false;

	lineThickness /= 2.f;

	fillRectangle({ rect.left - lineThickness, rect.top - lineThickness, rect.right + lineThickness, rect.top + lineThickness }, col); // left to right
	fillRectangle({ rect.left - lineThickness, rect.top - lineThickness, rect.left + lineThickness, rect.bottom + lineThickness }, col); // left to bottom
	fillRectangle({ rect.right - lineThickness, rect.top - lineThickness, rect.right + lineThickness, rect.bottom + lineThickness }, col); // right to bottom
	fillRectangle({ rect.left - lineThickness, rect.bottom - lineThickness, rect.right + lineThickness, rect.bottom + lineThickness }, col); // bottomleft to bottomright
	immediate = oImm;
	if (immediate) {
		SDK::MeshHelpers::renderMeshImmediately(scn, scn->tess, SDK::MaterialPtr::getUIColor());
	}
}

void MCDrawUtil::fillRoundedRectangle(RectF const& rect, d2d::Color const& color, float radius) {
	fillRectangle(rect, color);
}

void MCDrawUtil::drawRoundedRectangle(RectF rect, d2d::Color const& color, float radius, float lineThickness, OutlinePosition outPos) {
	drawRectangle(rect, color, lineThickness);
}

void MCDrawUtil::drawText(RectF const& rc, std::wstring const& text, d2d::Color const& color, Renderer::FontSelection font, float size, DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT verticalAlign, bool cache) {
	static uintptr_t caretMeasure = 0xFFFFFFFF;

	float newTop = rc.top;
	float vSize = (size * guiScale) / this->font->getLineHeight();

	switch (verticalAlign) {
	case DWRITE_PARAGRAPH_ALIGNMENT_CENTER:
		newTop = rc.centerY((this->font->getLineHeight() * vSize) / guiScale);
		break;
	case DWRITE_PARAGRAPH_ALIGNMENT_FAR:
		newTop = rc.bottom - (size);
		break;
	case DWRITE_PARAGRAPH_ALIGNMENT_NEAR:
		break;
	}
	
	RectF rMod = rc;
	rMod.top = newTop;
	renderCtx->drawText(this->font, getRect(rMod), util::WStrToStr(text), color, color.a, (SDK::ui::TextAlignment)alignment, SDK::TextMeasureData((size * guiScale) / this->font->getLineHeight(), false, false), &caretMeasure);
}

Vec2 MCDrawUtil::getTextSize(std::wstring const& text, Renderer::FontSelection font, float size, bool trailingWhitespace, bool cache) {
	return { this->font->getLineLength(util::WStrToStr(text), (size * guiScale) / this->font->getLineHeight(), trailingWhitespace) / guiScale, this->font->getLineHeight() * size * guiScale};
}

DrawUtil::RectF MCDrawUtil::getTextRect(std::wstring const& text, Renderer::FontSelection font, float size, float pad, bool cache) {
	return {};
}
