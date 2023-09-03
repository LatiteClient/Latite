#include "pch.h"
#include "TextBox.h"

bool ui::TextBox::isSelected() {
	return isSelectedBool;
}

void ui::TextBox::setSelected(bool b) {
	this->isSelectedBool = b;
	if (b) {
		startTime = std::chrono::high_resolution_clock::now();
	}
}

bool ui::TextBox::shouldBlink() {
	auto time = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime);
	return (diff.count() % 1000) < 500;
}

void ui::TextBox::onChar(char character) {
	if (character == '\b' && text.size() > 0) {
		place = std::max(place - 1, 0);
		text.erase(text.begin() + std::min(static_cast<int>(text.size()), place));
	}
	else if (character > 31 && text.size() < maxChars) {
		text.insert(text.begin() + std::min(static_cast<int>(text.size()), place), character);
		place++;
	}
	startTime = std::chrono::high_resolution_clock::now();
}

void ui::TextBox::onKeyDown(int key) {
	if (key == VK_LEFT) {
		place = std::max(place - 1, 0);
		startTime = std::chrono::high_resolution_clock::now();
	}
	else if (key == VK_RIGHT) {
		place = std::min(place + 1, maxChars);
		startTime = std::chrono::high_resolution_clock::now();
	}
}

void ui::TextBox::setText(std::string const& str) {
	text = str;
	if (place > str.size()) place = static_cast<int>(str.size());
}

void ui::TextBox::reset() {
	text.clear();
	place = 0;
}

d2d::Rect ui::TextBox::getRect() {
	return this->rect;
}

void ui::TextBox::render(DXContext& dc, float rounding, d2d::Color backgroundColor, d2d::Color textColor) {
	if (rounding == 0.f) {
		dc.fillRectangle(rect, backgroundColor);
	}
	else {
		dc.fillRoundedRectangle(rect, backgroundColor, rounding);
	}

	// draw text
	float textSize = rect.getHeight() * 0.7f;
	dc.drawText(rect, util::StrToWStr(getText()), textColor, Renderer::FontSelection::SegoeRegular, textSize,
		DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, false); // Don't cache

	// draw blinker
	Vec2 ts = dc.getTextSize(util::StrToWStr(text.substr(0, this->place)), Renderer::FontSelection::SegoeRegular, textSize);
	d2d::Rect blinkerRect = { rect.left + ts.x, rect.top + 2.f, rect.left + ts.x + 2.f, rect.bottom - 2.f };
	if (isSelected() && shouldBlink()) dc.fillRectangle(blinkerRect, textColor);
}
