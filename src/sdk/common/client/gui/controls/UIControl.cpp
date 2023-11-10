#include "pch.h"
#include "UIControl.h"

#include "sdk/signature/storage.h"

void SDK::UIControl::updatePos() {
	reinterpret_cast<void(*)(UIControl*)>(Signatures::UIControl_setPosition.result)(this);
}
