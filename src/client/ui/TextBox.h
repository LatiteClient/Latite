#pragma once
#include "util/Util.h"
#include "util/DxUtil.h"
#include "util/DxContext.h"
#ifdef min
#undef min
#undef max
#endif

namespace ui {
	class TextBox {
	public:
		[[nodiscard]] bool isSelected() {
			return isSelectedBool;
		}

		void setSelected(bool b) {
			this->isSelectedBool = b;
			if (b) {
				startTime = std::chrono::high_resolution_clock::now();
			}
		}

		[[nodiscard]] bool shouldBlink() {
			auto time = std::chrono::high_resolution_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time - startTime);
			return (diff.count() % 1000) < 500;
		}

		void onChar(char character) {
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

		void onKeyDown(int key) {
			if (key == VK_LEFT) {
				place = std::max(place - 1, 0);
				startTime = std::chrono::high_resolution_clock::now();
			}
			else if (key == VK_RIGHT) {
				place = std::min(place + 1, maxChars);
				startTime = std::chrono::high_resolution_clock::now();
			}
		}

		[[nodiscard]] std::string getText() {
			return text;
		}

		void setText(std::string const& str) {
			text = str;
			if (place > str.size()) place = static_cast<int>(str.size());
		}

		void reset() {
			text.clear();
			place = 0;
		}

		void setRect(d2d::Rect const& rect) {
			this->rect = rect;
		}

		[[nodiscard]] d2d::Rect getRect() {
			return this->rect;
		}

		TextBox(d2d::Rect rect, int maxChars = 32) : rect(rect), maxChars(maxChars), startTime(std::chrono::high_resolution_clock::now()) {}
		TextBox() : startTime(std::chrono::high_resolution_clock::now()) {};

		void render(DXContext& dc, float rounding, d2d::Color backgroundColor, d2d::Color textColor);

		[[nodiscard]] int getCaretLocation() { return place; }
	private:
		d2d::Rect rect = { 0, 0, 0, 0 };
		std::chrono::high_resolution_clock::time_point startTime;
		int maxChars = 32;
		int place = 0;
		std::string text = "";
		bool isSelectedBool = false;
	};
}
