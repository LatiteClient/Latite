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
		Chakra::DefineFunc(prototype, defaultToString, L"toString", this);
		Chakra::DefineFunc(prototype, entityIsValid, L"isValid", this);
		Chakra::DefineFunc(prototype, entityGetPos, L"getPosition", this);
		Chakra::DefineFunc(prototype, entityGetRot, L"getRotation", this);
		Chakra::DefineFunc(prototype, entityIsPlayer, L"isPlayer", this);
		Chakra::DefineFunc(prototype, entityIsLocalPlayer, L"isLocalPlayer", this);
		Chakra::DefineFunc(prototype, entityGetHurtTime, L"getHurtTime", this);
		Chakra::DefineFunc(prototype, entityGetType, L"getEntityType", this);
		Chakra::DefineFunc(prototype, entityAttack, L"attack", this);
		Chakra::DefineFunc(prototype, entityGetHealth, L"getHealth", this);
		Chakra::DefineFunc(prototype, entityGetHunger, L"getHunger", this);
		Chakra::DefineFunc(prototype, entityGetSaturation, L"getSaturation", this);
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
};