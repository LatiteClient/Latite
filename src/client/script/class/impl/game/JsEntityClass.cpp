#include "pch.h"
#include "JsEntityClass.h"
#include "../JsVec3.h"
#include "../JsVec2.h"
#include "sdk/common/world/level/Dimension.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "../../../JsScript.h"

JsValueRef JsEntityClass::entityIsValid(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 1)) return Chakra::GetFalse();
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent) {
		return ent->validate() ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(L"Object is not an entity");
	return Chakra::GetFalse();
}

JsValueRef JsEntityClass::entityGetPos(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 1)) return Chakra::GetUndefined();
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (ent->level != JsEntity::AccessLevel::Restricted) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec3>();

			auto ret = cl->construct(actor->getPos());
			return ret;
		}
		else {
			Chakra::ThrowError(L"Access denied");
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetRot(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 1)) return Chakra::GetUndefined();
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (ent->level != JsEntity::AccessLevel::Restricted) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec2>();

			auto ret = cl->construct(actor->getRot());
			return ret;
		}
		else {
			Chakra::ThrowError(L"Access denied");
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetDimensionName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		auto dim = util::StrToWStr(actor->dimension->dimensionName);
		JsValueRef dimension;
		JS::JsPointerToString(dim.c_str(), dim.size(), &dimension);
		return dimension;
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetHurtTime(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return Chakra::MakeInt(actor->invulnerableTime);
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityIsPlayer(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return actor->isPlayer() ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityAttack(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		SDK::ClientInstance::get()->getLocalPlayer()->swing();
		SDK::ClientInstance::get()->getLocalPlayer()->gameMode->attack(actor);
		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetType(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return Chakra::MakeInt(actor->getEntityTypeID());
	}
	Chakra::ThrowError(L"Invalid entity");
	return Chakra::GetUndefined();
}
