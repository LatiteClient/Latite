#include "pch.h"
#include "JsEntityClass.h"
#include "../JsVec3.h"
#include "../JsVec2.h"
#include "JsItemStack.h"
#include "mc/common/world/level/Dimension.h"
#include "mc/common/client/player/LocalPlayer.h"
#include "mc/common/client/game/ClientInstance.h"
#include "../../../JsPlugin.h"

JsValueRef JsEntityClass::entityIsValid(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent) {
		return ent->validate() ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(L"Object is not an entity");
	return Chakra::GetFalse();
}

JsValueRef JsEntityClass::entityGetPos(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (ent->level != JsEntity::AccessLevel::Restricted || SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec3>();

			auto ret = cl->construct(actor->getPos());
			return ret;
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getPosition"));
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetRot(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (ent->level != JsEntity::AccessLevel::Restricted || SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec2>();

			auto ret = cl->construct(actor->getRot());
			return ret;
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getRotation"));
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetDimensionName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		auto dim = util::StrToWStr(actor->dimension->dimensionName);
		JsValueRef dimension;
		JS::JsPointerToString(dim.c_str(), dim.size(), &dimension);
		return dimension;
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetHurtTime(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return Chakra::MakeInt(actor->invulnerableTime);
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityIsPlayer(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return actor->isPlayer() ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityAttack(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && ent->getEntity() != SDK::ClientInstance::get()->getLocalPlayer() && SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
		auto actor = ent->getEntity();
		SDK::ClientInstance::get()->getLocalPlayer()->swing();
		SDK::ClientInstance::get()->getLocalPlayer()->gameMode->attack(actor);
		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetType(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return Chakra::MakeInt(actor->getEntityTypeID());
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityIsLocalPlayer(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		return Chakra::MakeInt(ent->runtimeId == 1);
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entityGetHealth(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		if (ent->level != JsEntity::AccessLevel::Restricted) {
			return Chakra::MakeDouble(static_cast<float>(ent->getEntity()->getHealth()));
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getHealth"));
			return JS_INVALID_REFERENCE;
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entityGetHunger(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		if (ent->level != JsEntity::AccessLevel::Restricted) {
			return Chakra::MakeDouble(static_cast<float>(ent->getEntity()->getHunger()));
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getHunger"));
			return JS_INVALID_REFERENCE;
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entityGetSaturation(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent->level != JsEntity::AccessLevel::Restricted) {
		return Chakra::MakeDouble(static_cast<float>(ent->getEntity()->getSaturation()));
	}
	else {
		Chakra::ThrowError(XW("Access denied, cannot use getSaturation"));
		return JS_INVALID_REFERENCE;
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entityGetVariable(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsString } })) return JS_INVALID_REFERENCE;

	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));

	auto str = util::WStrToStr(Chakra::GetString(arguments[1]));

	if (ent && ent->getEntity()) {
		for (auto& var : ent->getEntity()->molangVariableMap.mVariables) {
			if (var->mName.getString().starts_with(XOR_STRING("variable."))) {
				if (var->mName == str) {
					// create it
					JsValueRef obj;
					JS::JsCreateObject(&obj);
					Chakra::SetProperty(obj, util::StrToWStr(XOR_STRING("number")), Chakra::MakeDouble(var->mValue.mPOD.mFloat));
					return obj;
				}
			}
		}
		return Chakra::GetNull();
	}

	Chakra::ThrowError(XW("Invalid entity"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entitySetVariable(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsString }, {arguments[2], JsNumber}})) return JS_INVALID_REFERENCE;

#ifdef LATITE_DEBUG
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));

	auto str = util::WStrToStr(Chakra::GetString(arguments[1]));
	auto newVal = Chakra::GetNumber(arguments[2]);

	if (ent && ent->getEntity()) {
		for (auto& var : ent->getEntity()->molangVariableMap.mVariables) {
			if (var->mName.getString().starts_with(XOR_STRING("variable."))) {
				if (var->mName == str) {
					// create it
					var->mValue.mPOD.mFloat = static_cast<float>(newVal);
					return arguments[2];
				}
			}
		}
		return Chakra::GetNull();
	}

	Chakra::ThrowError(XW("Invalid entity"));
#else
	Chakra::ThrowError(XW("setMolangVariable is reserved"));
#endif
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entityGetPosInterpolated(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (ent->level != JsEntity::AccessLevel::Restricted || SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec3>();

			auto alpha = SDK::ClientInstance::get()->minecraft->timer->alpha;
			auto& pos = actor->getPos();
			auto& posOld = actor->getPosOld();

			Vec3 interPos = { std::lerp(posOld.x, pos.x, alpha), std::lerp(posOld.y, pos.y, alpha), std::lerp(posOld.z, pos.z, alpha) };
			return cl->construct(interPos);
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getPositionInterpolated"));
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetPosPrev(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (ent->level != JsEntity::AccessLevel::Restricted || SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec3>();

			auto ret = cl->construct(actor->getPosOld());
			return ret;
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getPreviousPosition"));
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetStatusFlag(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsNumber } })) return JS_INVALID_REFERENCE;

	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		return actor->getStatusFlag(Chakra::GetInt(arguments[1])) ? Chakra::GetTrue() : Chakra::GetFalse();
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return JS_INVALID_REFERENCE;
}

JsValueRef JsEntityClass::entitySetStatusFlag(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsNumber }, { arguments[2], JsBoolean }})) return JS_INVALID_REFERENCE;

	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate() && SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
		auto actor = ent->getEntity();
		actor->setStatusFlag(Chakra::GetInt(arguments[1]), Chakra::GetBool(arguments[2]));
		return Chakra::GetUndefined();
	}
	
	Chakra::ThrowError(XW("Access denied, cannot use setStatusFlag"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetArmorSlot(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsNumber } })) return JS_INVALID_REFERENCE;

	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));

	if (!ent) {
		Chakra::ThrowError(XW("Invalid entity"));
		return JS_INVALID_REFERENCE;
	}

	int slot = Chakra::GetInt(arguments[1]);

	if (slot < 0 || slot > 3) {
		Chakra::ThrowError(XW("Armor slot out of bounds [0-3]"));
		return JS_INVALID_REFERENCE;
	}

	return JsScript::getThis()->getClass<JsItemStack>()->construct(ent->getEntity()->getArmor(slot), false);
}

JsValueRef JsEntityClass::entitySetVelocity(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject } })) return JS_INVALID_REFERENCE;

	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();
		if (SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
			actor->getVelocity() = JsVec3::ToVec3(arguments[1]);
			return Chakra::GetUndefined();
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use setVelocity"));
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}

JsValueRef JsEntityClass::entityGetVelocity(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsEntity* ent = nullptr;
	JS::JsGetExternalData(arguments[0], reinterpret_cast<void**>(&ent));
	if (ent && ent->validate()) {
		auto actor = ent->getEntity();

		if (ent->level != JsEntity::AccessLevel::Restricted || SDK::ClientInstance::get()->getLocalPlayer()->getCommandPermissionLevel() > 1) {
			auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
			auto cl = thi->owner->getClass<JsVec3>();

			auto ret = cl->construct(actor->getVelocity());
			return ret;
		}
		else {
			Chakra::ThrowError(XW("Access denied, cannot use getVelocity"));
			return Chakra::GetUndefined();
		}
	}
	Chakra::ThrowError(XW("Invalid entity"));
	return Chakra::GetUndefined();
}
