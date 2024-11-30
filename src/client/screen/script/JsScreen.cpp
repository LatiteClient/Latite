#include "pch.h"
#include "JsScreen.h"

JsScreen::JsScreen(JsValueRef object, JsValueRef renderFunc) : renderFunc(renderFunc) {

	Chakra::GetStringProperty(object, L"name");
	key = Chakra::GetIntProperty(object, L"key");
	
}

void JsScreen::onRender(Event& ev)
{
}

void JsScreen::onEnable(bool ignoreAnims) {
}

void JsScreen::onDisable() {
}
