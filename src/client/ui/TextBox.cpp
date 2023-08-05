#include "TextBox.h"

void ui::TextBox::render(DXContext& dc, float rounding, d2d::Color backgroundColor, d2d::Color textColor) {
	if (rounding == 0.f) {
		dc.fillRectangle(rect, backgroundColor);
	}
	else {
		dc.fillRoundedRectangle(rect, backgroundColor, rounding);
	}

	// draw text
	float textSize = rect.getHeight() * 0.8f;
	dc.drawText(rect, util::StrToWStr(getText()), textColor, Renderer::FontSelection::Regular, rect.getHeight() * 0.8f,
		DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// draw blinker
	Vec2 ts = dc.getTextSize(util::StrToWStr(text.substr(0, this->place)), Renderer::FontSelection::Regular, textSize);
	d2d::Rect blinkerRect = { rect.left + ts.x - 2.f, rect.top + 2.f, rect.left + ts.x, rect.bottom - 2.f };
	if (shouldBlink()) dc.fillRectangle(blinkerRect, textColor);
}
