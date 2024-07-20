#pragma once
namespace SDK {
	class ScreenView {
	public:
		CLASS_FIELD(class ScreenController*, screenController, 0x38);
		CLASS_FIELD(class VisualTree*, visualTree, 0x48);
	}; //Size: 0x0080
}