#include "pch.h"
#include "TextBox.h"

bool TextBox::isSelected() {
	return isSelectedBool;
}

void TextBox::setSelected(bool b) {
	this->isSelectedBool = b;
	if (b) {
		startTime = std::chrono::high_resolution_clock::now();
	}
}

bool TextBox::shouldBlink() {
	auto time = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime);
	return (diff.count() % 1000) < 500;
}

void TextBox::onChar(wchar_t character) {
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

void TextBox::onKeyDown(int key) {
	if (key == VK_LEFT) {
		place = std::max(place - 1, 0);
		startTime = std::chrono::high_resolution_clock::now();
	}
	else if (key == VK_RIGHT) {
		place = std::min(place + 1, maxChars);
		startTime = std::chrono::high_resolution_clock::now();
	}
}

void TextBox::setText(std::wstring const& str) {
	text = str;
	if (place > str.size()) place = static_cast<int>(str.size());
}

void TextBox::reset() {
	text.clear();
	place = 0;
}

d2d::Rect TextBox::getRect() {
	return this->rect;
}

void TextBox::render(DrawUtil& dc, float rounding, d2d::Color backgroundColor, d2d::Color textColor) {
	if (rounding == 0.f) {
		dc.fillRectangle(rect, backgroundColor);
	}
	else {
		dc.fillRoundedRectangle(rect, backgroundColor, rounding);
	}

	// draw text
	float textSize = rect.getHeight() * 0.7f;
	dc.drawText(rect, getText(), textColor, Renderer::FontSelection::PrimaryRegular, textSize,
		DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, false); // Don't cache

	// draw blinker
	Vec2 ts = dc.getTextSize(text.substr(0, this->place), Renderer::FontSelection::PrimaryRegular, textSize);
	d2d::Rect blinkerRect = { rect.left + ts.x, rect.top + 2.f, rect.left + ts.x + 2.f, rect.bottom - 2.f };
	if (isSelected() && shouldBlink()) dc.fillRectangle(blinkerRect, textColor);
}
