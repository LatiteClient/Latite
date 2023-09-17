#pragma once
#include "util/Util.h"
#include "util/Dxutil.h"
#ifdef min
#undef min
#undef max
#endif

class DrawUtil;

namespace ui {
	class TextBox {
	public:
		[[nodiscard]] bool isSelected();

		void setSelected(bool b);

		[[nodiscard]] bool shouldBlink();

		void onChar(wchar_t character);

		void onKeyDown(int key);

		[[nodiscard]] std::wstring getText() {
			return text;
		}

		void setText(std::wstring const& str);

		void reset();

		void setRect(d2d::Rect const& rect) {
			this->rect = rect;
		}

		[[nodiscard]] d2d::Rect getRect();

		TextBox(d2d::Rect rect, int maxChars = 32) : rect(rect), maxChars(maxChars), startTime(std::chrono::high_resolution_clock::now()) {}
		TextBox() : startTime(std::chrono::high_resolution_clock::now()) {};

		void render(DrawUtil& dc, float rounding, d2d::Color backgroundColor, d2d::Color textColor);

		[[nodiscard]] int getCaretLocation() { return place; }
		void setCaretLocation(int loc) { place = loc; }
	private:
		d2d::Rect rect = { 0, 0, 0, 0 };
		std::chrono::high_resolution_clock::time_point startTime;
		int maxChars = 32;
		int place = 0;
		std::wstring text;
		bool isSelectedBool = false;
	};
}
