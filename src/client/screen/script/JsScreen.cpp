#include "pch.h"
#include "JsScreen.h"

JsScreen::JsScreen(JsValueRef object) {

	Chakra::GetStringProperty(object, L"name");
	key = Chakra::GetIntProperty(object, L"key");
	
}

void JsScreen::onEnable(bool ignoreAnims) {
}

void JsScreen::onDisable() {
}
