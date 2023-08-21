#pragma once
#include <string>

namespace SDK {
	class ScreenView
	{
		[[maybe_unused]] char pad_0000[72]; //0x0000
	public:
		class VisualTree* visualTree; //0x0048
	}; //Size: 0x0080

	class UIControl
	{
		[[maybe_unused]] char pad_0000[24]; //0x0000
	public:
		std::string name; //0x0018
	};

	class VisualTree {
		[[maybe_unused]] char pad[8];
	public:
		class UIControl* rootControl;
	};
}