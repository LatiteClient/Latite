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
	
	auto setMoveState = [&]<typename T>(T& state, const std::wstring& name) {
		if (auto prop = Chakra::GetProperty(obj, name)) {
			if constexpr (std::is_same_v<bool, T>) {
				state = Chakra::GetBool(prop);
			}
			else if constexpr (std::is_floating_point_v<T>) {
				state = Chakra::GetNumber(prop);
			}
		}
		};

	// Can't be used anymore as the fields are bits instead of bytes, so no reference possible
	/*setMoveState(move->rawInputState.up, L"forward");
	setMoveState(move->rawInputState.down, L"backward");
	setMoveState(move->rawInputState.left, L"left");
	setMoveState(move->rawInputState.right, L"right");
	setMoveState(move->rawInputState.jumpDown, L"jump");
	setMoveState(move->rawInputState.sneakDown, L"sneak");*/

	if (auto prop = Chakra::GetProperty(obj, L"forward")) {
		move->rawInputState.up = Chakra::GetBool(prop);
	}

	if (auto prop = Chakra::GetProperty(obj, L"backward")) {
		move->rawInputState.down = Chakra::GetBool(prop);
	}

	if (auto prop = Chakra::GetProperty(obj, L"left")) {
		move->rawInputState.left = Chakra::GetBool(prop);
	}

	if (auto prop = Chakra::GetProperty(obj, L"right")) {
		move->rawInputState.right = Chakra::GetBool(prop);
	}

	if (auto prop = Chakra::GetProperty(obj, L"jump")) {
		move->rawInputState.jumpDown = Chakra::GetBool(prop);
	}

	if (auto prop = Chakra::GetProperty(obj, L"sneak")) {
		move->rawInputState.sneakDown = Chakra::GetBool(prop);
	}
	
	setMoveState(move->rawInputState.analogMoveVector.x, L"joystickX");
	setMoveState(move->rawInputState.analogMoveVector.y, L"joystickY");
	
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
	Chakra::SetPropertyBool(obj, L"forward", move->rawInputState.up);
	Chakra::SetPropertyBool(obj, L"backward", move->rawInputState.down);
	Chakra::SetPropertyBool(obj, L"left", move->rawInputState.left);
	Chakra::SetPropertyBool(obj, L"right", move->rawInputState.right);
	Chakra::SetPropertyBool(obj, L"jump", move->rawInputState.jumpDown);
	Chakra::SetPropertyBool(obj, L"sneak", move->rawInputState.sneakDown);
	Chakra::SetPropertyBool(obj, L"sprintHeld", move->rawInputState.sprintDown);
	Chakra::SetPropertyNumber(obj, L"joystickX", move->rawInputState.analogMoveVector.x);
	Chakra::SetPropertyNumber(obj, L"joystickY", move->rawInputState.analogMoveVector.y);

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
