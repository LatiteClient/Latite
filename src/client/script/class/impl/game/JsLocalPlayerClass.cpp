#include "pch.h"
#include "JsLocalPlayerClass.h"
#include <sdk/common/world/level/HitResult.h>
#include <client/script/class/impl/JsVec3.h>
#include <client/script/class/impl/JsVec2.h>

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

	return JsScript::getThis()->getClass<JsVec3>()->construct(Vec3{static_cast<float>(hr->hitBlock.x), static_cast<float>(hr->hitBlock.y), static_cast<float>(hr->hitBlock.z)});
}

JsValueRef JsLocalPlayerClass::setMovementState(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsObject} })) return JS_INVALID_REFERENCE;
	
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}
	
	JsValueRef obj = arguments[1];

	auto move = lp->getMoveInputComponent();
	
	auto setMoveState = [&](bool& state, const std::wstring& name) {
		if (auto prop = Chakra::GetProperty(obj, name)) {
			state = Chakra::GetBool(prop);
		}
		};

	setMoveState(move->front, L"forward");
	setMoveState(move->back, L"backward");
	setMoveState(move->left, L"left");
	setMoveState(move->right, L"right");
	setMoveState(move->jump, L"jump");
	setMoveState(move->sneak, L"sneak");
	
	return obj;
}

JsValueRef JsLocalPlayerClass::getMovementState(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}

	JsValueRef obj = JS_INVALID_REFERENCE;
	JS::JsCreateObject(&obj);

	auto move = lp->getMoveInputComponent();
	Chakra::SetPropertyBool(obj, L"forward", move->front);
	Chakra::SetPropertyBool(obj, L"backward", move->back);
	Chakra::SetPropertyBool(obj, L"left", move->left);
	Chakra::SetPropertyBool(obj, L"right", move->right);
	Chakra::SetPropertyBool(obj, L"jump", move->jump);
	Chakra::SetPropertyBool(obj, L"sneak", move->sneak);
	Chakra::SetPropertyBool(obj, L"sprintHeld", move->sprintKey);

	return obj;
}

JsValueRef JsLocalPlayerClass::turn(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsObject} })) return JS_INVALID_REFERENCE;
	
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) {
		Chakra::ThrowError(XW("Invalid local player"));
		return JS_INVALID_REFERENCE;
	}

	Vec2 delta = JsVec2::ToVec2(arguments[1]);
	lp->applyTurnDelta(delta);

	return Chakra::GetUndefined();
}
