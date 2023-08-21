#pragma once
namespace SDK {
	class ScreenView
	{
		char pad_0000[72]; //0x0000
	public:
		class VisualTree* visualTree; //0x0048
	}; //Size: 0x0080
}