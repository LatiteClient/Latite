#include "pch.h"
#include "JsPlayerClass.h"
#include "JsItemStack.h"
#include "../../../JsPlugin.h"

JsValueRef JsPlayerClass::playerGetName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && ent->getEntity()->isPlayer()) {
		return Chakra::MakeString(util::StrToWStr(static_cast<SDK::Player*>(ent->getEntity())->playerName));
	}
	Chakra::ThrowError(XW("Invalid player"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsPlayerClass::playerGetXboxUserID(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && ent->getEntity()->isPlayer()) {
		return Chakra::MakeString(util::StrToWStr(static_cast<SDK::Player*>(ent->getEntity())->getXUID()));
	}
	Chakra::ThrowError(XW("Invalid player"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsPlayerClass::playerGetSelectedItem(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && ent->getEntity()->isPlayer()) {
		if (ent->level != JsEntity::AccessLevel::Restricted) {
			auto supp = static_cast<SDK::Player*>(ent->getEntity())->supplies;
			return JsScript::getThis()->getClass<JsItemStack>()->construct(supp->inventory->getItem(supp->selectedSlot), false);
		}
		else {
			Chakra::ThrowError(XW("Access denied - cannot use getHoldingItem"));
			return JS_INVALID_REFERENCE;
		}
	}
	Chakra::ThrowError(XW("Invalid player"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsPlayerClass::playerGetSelectedSlot(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && ent->getEntity()->isPlayer()) {
		if (ent->level != JsEntity::AccessLevel::Restricted) {
			auto supp = static_cast<SDK::Player*>(ent->getEntity())->supplies;
			return Chakra::MakeInt(supp->selectedSlot);
		}
		else {
			Chakra::ThrowError(XW("Access denied - cannot use getHoldingItem"));
			return JS_INVALID_REFERENCE;
		}
	}
	Chakra::ThrowError(XW("Invalid player"));
	return JS_INVALID_REFERENCE;
}
