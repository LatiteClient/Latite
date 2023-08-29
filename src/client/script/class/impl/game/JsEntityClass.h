#pragma once
#include "client/script/interop/classes/JsEntity.h"
#include "../../JsClass.h"

class JsEntityClass : public JsClass {
public:
	static JsValueRef CALLBACK jsConstructor(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsEntityClass*>(callbackState);

		if (!isConstructor) return thi->errConstructCall();

		Chakra::ThrowError(thi->name + std::wstring(L" cannot be constructed"));
		return JS_INVALID_REFERENCE;
	}

	inline static const wchar_t* class_name = L"Entity";
	JsEntityClass(class JsScript* owner, const wchar_t* name = class_name) : JsClass(owner, name) {
		createConstructor(jsConstructor, this);
	}

	virtual JsValueRef construct(JsEntity* ent, bool finalize) {
		JsValueRef obj;
		if (finalize) {
			JS::JsCreateExternalObject(ent, [](void* obj) {
				delete obj;
				}, &obj);
		}
		else {
			JS::JsCreateExternalObject(ent, [](void*) {
				}, &obj);
		}
		JS::JsSetPrototype(obj, getPrototype());
		return obj;
	}

	void prepareFunctions() override {
		Chakra::DefineFunc(prototype, toStringCallback, L"toString", this);
		Chakra::DefineFunc(prototype, entityIsValid, L"isValid", this);
		Chakra::DefineFunc(prototype, entityGetPos, L"getPosition", this);
		Chakra::DefineFunc(prototype, entityGetRot, L"getRotation", this);
		Chakra::DefineFunc(prototype, entityIsPlayer, L"isPlayer", this);
		Chakra::DefineFunc(prototype, entityGetHurtTime, L"getHurtTime", this);
		Chakra::DefineFunc(prototype, entityGetType, L"getEntityType", this);
		Chakra::DefineFunc(prototype, entityAttack, L"attack", this);
	};

	static JsValueRef CALLBACK toStringCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState) {
		auto thi = reinterpret_cast<JsEntityClass*>(callbackState);
		std::string add = std::format("{}", util::WStrToStr(thi->name));
		return Chakra::MakeString(L"[object " + util::StrToWStr(add) + L"]");
	}

	static JsEntity* ToEntity(JsValueRef obj) {
		JsEntity* mod = nullptr;
		JS::JsGetExternalData(obj, reinterpret_cast<void**>(&mod));
		return mod;
	}
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
};