#include "pch.h"
#include "JsLocalPlayerClass.h"
#include <sdk/common/world/level/HitResult.h>
#include <client/script/class/impl/JsVec3.h>

JsValueRef JsLocalPlayerClass::getBreakProgress(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}
	return Chakra::MakeDouble(lp->gameMode->breakProgress);
}

JsValueRef JsLocalPlayerClass::getLastBreakProgress(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}
	return Chakra::MakeDouble(lp->gameMode->lastBreakProgress);
}

JsValueRef JsLocalPlayerClass::getLookingAt(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}
	return Chakra::MakeInt((int)SDK::ClientInstance::get()->minecraft->getLevel()->getHitResult()->hitType);
}

JsValueRef JsLocalPlayerClass::getSelectedBlock(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}

	auto hr = SDK::ClientInstance::get()->minecraft->getLevel()->getHitResult();

	if (hr->hitType != SDK::HitType::BLOCK) {
		return Chakra::GetNull();
	}

	return JsScript::getThis()->getClass<JsVec3>()->construct(Vec3{hr->hitBlock.x, hr->hitBlock.y, hr->hitBlock.z});
}
