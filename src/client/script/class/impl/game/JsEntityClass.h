#pragma once
#include "client/script/interop/classes/JsEntity.h"
#include "../../JsWrapperClass.h"

class JsEntityClass : public JsWrapperClass<JsEntity> {
public:
	inline static const wchar_t* class_name = L"Entity";
	JsEntityClass(class JsScript* owner, const wchar_t* name = class_name) : JsWrapperClass(owner, name) {
		createConstructor(jsNoConstructor, this);
	}

	virtual JsValueRef construct(JsEntity* ent, bool finalize) override {
		auto obj = JsWrapperClass::construct(ent, finalize);
		JS::JsSetPrototype(obj, getPrototype());
		Chakra::SetProperty(obj, L"runtimeId", Chakra::MakeDouble(static_cast<double>(ent->runtimeId)));
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, defaultToString, XW("toString"), this);
		Chakra::DefineFunc(prototype, entityIsValid, XW("isValid"), this);
		Chakra::DefineFunc(prototype, entityGetPos, XW("getPosition"), this);
		Chakra::DefineFunc(prototype, entityGetPosPrev, XW("getPreviousPosition"), this);
		Chakra::DefineFunc(prototype, entityGetPosInterpolated, XW("getPositionInterpolated"), this);
		Chakra::DefineFunc(prototype, entityGetRot, XW("getRotation"), this);
		Chakra::DefineFunc(prototype, entityIsPlayer, XW("isPlayer"), this);
		Chakra::DefineFunc(prototype, entityIsLocalPlayer, XW("isLocalPlayer"), this);
		Chakra::DefineFunc(prototype, entityGetHurtTime, XW("getHurtTime"), this);
		Chakra::DefineFunc(prototype, entityGetType, XW("getEntityType"), this);
		Chakra::DefineFunc(prototype, entityAttack, XW("attack"), this);
		Chakra::DefineFunc(prototype, entityGetHealth, XW("getHealth"), this);
		Chakra::DefineFunc(prototype, entityGetHunger, XW("getHunger"), this);
		Chakra::DefineFunc(prototype, entityGetSaturation, XW("getSaturation"), this);
		Chakra::DefineFunc(prototype, entityGetVariable, XW("getMolangVariable"), this);
		Chakra::DefineFunc(prototype, entitySetVariable, XW("setMolangVariable"), this);
		Chakra::DefineFunc(prototype, entityGetStatusFlag, XW("getFlag"), this);
		Chakra::DefineFunc(prototype, entitySetStatusFlag, XW("setFlag"), this);
		Chakra::DefineFunc(prototype, entityGetArmorSlot, XW("getArmorSlot"), this);
		Chakra::DefineFunc(prototype, entityGetVelocity, XW("getVelocity"), this);
		Chakra::DefineFunc(prototype, entitySetVelocity, XW("setVelocity"), this);
		Chakra::SetPropertyNumber(prototype, L"runtimeId", 0.0, true);
	};
private:
	static JsValueRef CALLBACK entityIsValid(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetPos(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetRot(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetDimensionName(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetHurtTime(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityIsPlayer(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityAttack(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetType(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityIsLocalPlayer(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetHealth(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetHunger(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetSaturation(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetVariable(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entitySetVariable(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetPosInterpolated(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetPosPrev(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetStatusFlag(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entitySetStatusFlag(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetArmorSlot(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entityGetVelocity(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK entitySetVelocity(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};