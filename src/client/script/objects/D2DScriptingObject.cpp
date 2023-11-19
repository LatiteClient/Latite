#include "pch.h"
#include "D2DScriptingObject.h"
#include "../class/impl/JsColor.h"
#include "../class/impl/JsRect.h"
#include "../class/impl/JsVec2.h"
#include "util/DxContext.h"
#include <client/script/class/impl/JsTextureClass.h>

void D2DScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, useCallback, L"use", this);
	Chakra::DefineFunc(object, fillRectCallback, L"fillRect", this);
	Chakra::DefineFunc(object, drawRectCallback, L"drawRect", this);
	Chakra::DefineFunc(object, drawTextCallback, L"drawText", this);
	Chakra::DefineFunc(object, drawTextFullCallback, L"drawTextFull", this);
	Chakra::DefineFunc(object, drawImageCallback, L"drawTexture", this);
}

void D2DScriptingObject::onRenderOverlay(Event& ev) {
	auto mLock = this->lock();
	flushOverlay();
}

void D2DScriptingObject::onRenderLayer(Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	this->cachedCtx = ev.getUIRenderContext();
}

void D2DScriptingObject::flushOverlay() {
	while (!this->operations.empty()) {
		auto& latest = operations.front();
		auto ctx = Latite::getRenderer().getDeviceContext();
		D2D1::Matrix3x2F oMat;
		ctx->GetTransform(&oMat);
		ctx->SetTransform(latest.matrix);
		std::visit(DrawVisitor{}, latest.op);
		ctx->SetTransform(oMat);
		operations.pop();
	}
	Latite::getRenderer().getDeviceContext()->Flush();
}

JsValueRef D2DScriptingObject::useCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsString }})) return JS_INVALID_REFERENCE;

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);

	auto str = Chakra::GetString(arguments[1]);
	if (str == L"dx") {
		thi->setUseMinecraftRend(false);
	}
	else if (str == L"minecraft") {
		thi->setUseMinecraftRend(true);
	}
	else {
		Chakra::ThrowError(L"Unknown renderer \"" + str + L"\"");
		return JS_INVALID_REFERENCE;
	}

	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::fillRectCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject } })) return JS_INVALID_REFERENCE;

	auto rect = JsRect::ToRect(arguments[1]);
	auto color = JsColor::ToColor(arguments[2]);

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx) {
		MCDrawUtil dc{thi->cachedCtx, Latite::get().getFont()};
		dc.fillRectangle(rect, color);
		return Chakra::GetUndefined();
	}

	auto mLock = thi->lock();
	thi->operations.push({ thi->matrix, OpFillRect(rect, color) });

	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawRectCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 4, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsNumber}})) return JS_INVALID_REFERENCE;

	auto rect = JsRect::ToRect(arguments[1]);
	auto color = JsColor::ToColor(arguments[2]);
	auto thickness = static_cast<float>(Chakra::GetNumber(arguments[2]));

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx) {
		MCDrawUtil dc{ thi->cachedCtx, Latite::get().getFont() };
		dc.drawRectangle(rect, color, thickness);
		return Chakra::GetUndefined();
	}

	auto mLock = thi->lock();
	thi->operations.push({thi->matrix, OpDrawRect(rect, color, thickness) });

	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawTextCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsString }, { arguments[3], JsNumber}, {arguments[4], JsObject}})) return JS_INVALID_REFERENCE;

	auto pos = JsVec2::ToVec2(arguments[1]);
	auto text = Chakra::GetString(arguments[2]);
	auto size = static_cast<float>(Chakra::GetNumber(arguments[3]));
	auto color = JsColor::ToColor(arguments[4]);

	auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;

	d2d::Rect rc = { pos.x, pos.y, ss.x, ss.y };

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx) {
		MCDrawUtil dc{ thi->cachedCtx, Latite::get().getFont() };
		dc.drawText(rc, text, color, Renderer::FontSelection::SegoeRegular, size, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);
		dc.flush(true, false);
		return Chakra::GetUndefined();
	}

	auto mLock = thi->lock();
	thi->operations.push({thi->matrix, OpDrawText(rc, text, size, color, Renderer::FontSelection::SegoeRegular, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR) });
	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawTextFullCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 7, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsString }, { arguments[3], JsNumber}, {arguments[4], JsObject}, {arguments[5], JsNumber}, {arguments[6], JsNumber}  })) return JS_INVALID_REFERENCE;

	auto rect = JsRect::ToRect(arguments[1]);
	auto text = Chakra::GetString(arguments[2]);
	auto size = static_cast<float>(Chakra::GetNumber(arguments[3]));
	auto color = JsColor::ToColor(arguments[4]);
	auto align = Chakra::GetInt(arguments[5]);
	auto vertAlign = Chakra::GetInt(arguments[6]);

	if ((align > 2 || align < 0) || (vertAlign > 2 || vertAlign < 0)) {
		Chakra::ThrowError(L"Invalid text alignment");
		return Chakra::GetUndefined();
	}

	auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx) {
		MCDrawUtil dc{ thi->cachedCtx, Latite::get().getFont() };
		dc.drawText(rect, text, color, Renderer::FontSelection::SegoeRegular, size, (DWRITE_TEXT_ALIGNMENT)align, (DWRITE_PARAGRAPH_ALIGNMENT)vertAlign, false);
		dc.flush(true, false);
		return Chakra::GetUndefined();
	}

	auto thisptr = reinterpret_cast<D2DScriptingObject*>(callbackState);
	auto mLock = thi->lock();
	thisptr->operations.push({ thi->matrix, OpDrawText(rect, text, size, color, Renderer::FontSelection::SegoeRegular,(DWRITE_TEXT_ALIGNMENT)align, (DWRITE_PARAGRAPH_ALIGNMENT)vertAlign) });
	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawImageCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 6, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsNumber }, {arguments[4], JsNumber}, { arguments[5], JsObject, true }})) return JS_INVALID_REFERENCE;

	auto texture = JsTextureClass::Get(arguments[1]);
	auto pos = JsVec2::ToVec2(arguments[2]);
	auto sx = (float)Chakra::GetNumber(arguments[3]);
	auto sy = (float)Chakra::GetNumber(arguments[4]);
	auto color = d2d::Colors::WHITE;

	if (auto jval = Chakra::TryGet(arguments, argCount, 5)) {
		color = JsColor::ToColor(jval);
	}
	
	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx && texture && texture->getTexture()) {
		MCDrawUtil dc{ thi->cachedCtx, Latite::get().getFont() };
		dc.drawImage(*texture->getTexture(), pos, { sx, sy }, color);
		return Chakra::GetUndefined();
	}

	thi->operations.push({ thi->matrix, OpDrawImage{texture, pos, sx, sy, color} });

	return Chakra::GetUndefined();
}

void D2DScriptingObject::DrawVisitor::operator()(OpDrawRect& op) {
	D2DUtil dc;
	dc.drawRectangle(op.rc, op.col, op.thickness);
}

void D2DScriptingObject::DrawVisitor::operator()(OpFillRect& op) {
	D2DUtil dc;
	dc.fillRectangle(op.rc, op.col);
}

void D2DScriptingObject::DrawVisitor::operator()(OpDrawText& op) {
	D2DUtil dc;
	dc.drawText(op.rect, op.text, op.col, op.font, op.size, op.alignment, op.vertAlign);
}

void D2DScriptingObject::DrawVisitor::operator()(OpDrawImage& op) {
	if (!op.texture->getBitmap()) return;
	D2DUtil dc;
	//dc.drawText(op.rect, op.text, op.col, op.font, op.size, op.alignment, op.vertAlign);
	dc.ctx->DrawBitmap(op.texture->getBitmap(), { op.pos.x, op.pos.y, op.pos.x + op.sx, op.pos.y + op.sy });
}
