#pragma once
#include "JsEntityClass.h"

class JsPlayerClass : public JsEntityClass {
public:
	inline static const wchar_t* class_name = L"Player";
	JsPlayerClass(class JsScript* owner, const wchar_t* name = class_name) : JsEntityClass(owner, name) {
	}

	//JsValueRef construct(JsEntity* ent, bool finalize) override{
	//	auto obj = __super::construct(ent, finalize);
	//	return obj;
	//}

	void prepareFunctions() override {
		__super::prepareFunctions();
		Chakra::DefineFunc(prototype, playerGetName, XW("getName"));
		Chakra::DefineFunc(prototype, playerGetXboxUserID, XW("getXUID"));
		Chakra::DefineFunc(prototype, playerGetSelectedItem, XW("getHoldingItem"));
		Chakra::DefineFunc(prototype, playerGetSelectedSlot, XW("getSelectedSlot"));
		Chakra::DefineFunc(prototype, playerGetInventorySlot, XW("getItem"));
	}
private:
	static JsValueRef CALLBACK playerGetName(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK playerGetXboxUserID(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK playerGetSelectedItem(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK playerGetSelectedSlot(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK playerGetInventorySlot(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};