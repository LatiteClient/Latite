#pragma once
#include "JsPlayerClass.h"

class JsLocalPlayerClass : public JsPlayerClass {
public:
	inline static const wchar_t* class_name = L"LocalPlayer";
	JsLocalPlayerClass(class JsScript* owner) : JsPlayerClass(owner, class_name) {
	}

	void prepareFunctions() override {
		__super::prepareFunctions();

		Chakra::DefineFunc(prototype, getBreakProgress, XW("getBreakProgress"), this);
		Chakra::DefineFunc(prototype, getLastBreakProgress, XW("getLastBreakProgress"), this);
		Chakra::DefineFunc(prototype, getLookingAt, XW("getLookingAt"), this);
		Chakra::DefineFunc(prototype, getSelectedBlock, XW("getSelectedBlock"), this);
		Chakra::DefineFunc(prototype, getMovementState, XW("getMovementState"), this);
		Chakra::DefineFunc(prototype, setMovementState, XW("setMovementState"), this);
	}
private:
	static JsValueRef CALLBACK getBreakProgress(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getLastBreakProgress(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getLookingAt(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getSelectedBlock(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK getMovementState(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK setMovementState(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};