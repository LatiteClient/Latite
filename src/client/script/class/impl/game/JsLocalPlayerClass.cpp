#include "pch.h"
#include "JsLocalPlayerClass.h"

JsValueRef JsLocalPlayerClass::getBreakProgress(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(L"Invalid local player");
		return JS_INVALID_REFERENCE;
	}
	return Chakra::MakeDouble(lp->gameMode->breakProgress);
}

JsValueRef JsLocalPlayerClass::getLastBreakProgress(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(L"Invalid local player");
		return JS_INVALID_REFERENCE;
	}
	return Chakra::MakeDouble(lp->gameMode->lastBreakProgress);
}
