#pragma once
#include "JsPlayerClass.h"

class JsLocalPlayerClass : public JsPlayerClass {
public:
	inline static const wchar_t* class_name = L"LocalPlayer";
	JsLocalPlayerClass(class JsScript* owner) : JsPlayerClass(owner, class_name) {
	}

	void prepareFunctions() override {
		__super::prepareFunctions();

		Chakra::DefineFunc(prototype, getBreakProgress, L"getBreakProgress", this);
		Chakra::DefineFunc(prototype, getLastBreakProgress, L"getLastBreakProgress", this);
		Chakra::DefineFunc(prototype, getLookingAt, L"getLookingAt", this);
		Chakra::DefineFunc(prototype, getSelectedBlock, L"getSelectedBlock", this);
		Chakra::DefineFunc(prototype, getMovementState, L"getMovementState", this);
		Chakra::DefineFunc(prototype, setMovementState, L"setMovementState", this);
		Chakra::DefineFunc(prototype, turn, L"turn", this);
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
	static JsValueRef CALLBACK turn(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};