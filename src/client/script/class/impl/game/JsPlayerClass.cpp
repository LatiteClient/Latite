#include "pch.h"
#include "JsPlayerClass.h"

JsValueRef JsPlayerClass::playerGetName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && ent->getEntity()->isPlayer()) {
		return Chakra::MakeString(util::StrToWStr(static_cast<SDK::Player*>(ent->getEntity())->playerName));
	}
	Chakra::ThrowError(L"Invalid player");
	return JS_INVALID_REFERENCE;
}
