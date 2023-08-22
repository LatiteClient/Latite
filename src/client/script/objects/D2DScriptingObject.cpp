#include "D2DScriptingObject.h"
#include "../class/impl/JsColor.h"
#include "../class/impl/JsRect.h"
#include "util/DxContext.h"

void D2DScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, fillRectCallback, L"fillRect", this);
	Chakra::DefineFunc(object, drawRectCallback, L"drawRect", this);
}

void D2DScriptingObject::onRenderOverlay(Event& ev) {
	while (!this->operations.empty()) {
		auto& latest = operations.front();
		std::visit(DrawVisitor{}, latest.op);
		operations.pop();
	}
}

JsValueRef D2DScriptingObject::fillRectCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject } })) return JS_INVALID_REFERENCE;

	auto rect = JsRect::ToRect(arguments[1]);
	auto color = JsColor::ToColor(arguments[2]);

	auto thisptr = reinterpret_cast<D2DScriptingObject*>(callbackState);
	thisptr->operations.push({ OpFillRect(rect, color) });

	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawRectCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 4, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsNumber}})) return JS_INVALID_REFERENCE;

	auto rect = JsRect::ToRect(arguments[1]);
	auto color = JsColor::ToColor(arguments[2]);
	auto thickness = static_cast<float>(Chakra::GetNumber(arguments[2]));

	auto thisptr = reinterpret_cast<D2DScriptingObject*>(callbackState);
	thisptr->operations.push({ OpDrawRect(rect, color, thickness) });

	return Chakra::GetUndefined();
}

void D2DScriptingObject::DrawVisitor::operator()(OpDrawRect& op) {
	DXContext dc;
	dc.drawRectangle(op.rc, op.col, op.thickness);
}

void D2DScriptingObject::DrawVisitor::operator()(OpFillRect& op) {
	DXContext dc;
	dc.fillRectangle(op.rc, op.col);
}

void D2DScriptingObject::DrawVisitor::operator()(OpDrawText& op) {
	DXContext dc;
	dc.drawText(op.rect, op.text, op.col, op.font, op.size, op.alignment, op.vertAlign);
}
