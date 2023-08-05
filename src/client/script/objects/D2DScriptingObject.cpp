#include "D2DScriptingObject.h"
#include "../class/impl/JsColor.h"
#include "../class/impl/JsRect.h"

void D2DScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, fillRectCallback, L"fillRect", this);
}

JsValueRef D2DScriptingObject::fillRectCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject } }));

	auto rect = JsRect::ToRect(arguments[1]);
	auto color = JsColor::ToColor(arguments[2]);

	auto thisptr = reinterpret_cast<D2DScriptingObject*>(callbackState);
	thisptr->operations.push({ OpFillRect(rect, color) });

	return Chakra::GetUndefined();
}
