#include "pch.h"
#include "D2DScriptingObject.h"
#include "../class/impl/JsColor.h"
#include "../class/impl/JsRect.h"
#include "../class/impl/JsVec2.h"
#include "util/DxContext.h"
#include <client/script/class/impl/JsTextureClass.h>
#include <client/script/class/impl/game/JsItemStack.h>

void D2DScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, useCallback, XW("use"), this);
	Chakra::DefineFunc(object, fillRectCallback, XW("fillRect"), this);
	Chakra::DefineFunc(object, drawRectCallback, XW("drawRect"), this);
	Chakra::DefineFunc(object, drawTextCallback, XW("drawText"), this);
	Chakra::DefineFunc(object, drawTextFullCallback, XW("drawTextFull"), this);
	Chakra::DefineFunc(object, drawImageCallback, XW("drawTexture"), this);
	Chakra::DefineFunc(object, getTextSize, XW("getTextSize"), this);
	Chakra::DefineFunc(object, setClippingRect, XW("setClipRect"), this);
	Chakra::DefineFunc(object, restoreClippingRect, XW("restoreClipRect"), this);
	Chakra::DefineFunc(object, drawItem, XW("drawItem"), this);
}

void D2DScriptingObject::onRenderOverlay(Event& ev) {
	auto mLock = this->lock();
	Latite::getRenderer().getDeviceContext()->GetTransform(&matrix);
	flushOverlay();
}

void D2DScriptingObject::onRenderLayer(Event& evG) {
	auto& ev = reinterpret_cast<RenderLayerEvent&>(evG);
	this->cachedCtx = ev.getUIRenderContext();
}

void D2DScriptingObject::onUpdate(Event&) {
	auto lk = lock();
	operations = operationsDirty;
	operationsDirty.clear();
}

void D2DScriptingObject::flushOverlay() {
	auto lk = lock();
	for (auto& operation : operations) {
		auto ctx = Latite::getRenderer().getDeviceContext();
		D2D1::Matrix3x2F oMat;
		ctx->GetTransform(&oMat);
		ctx->SetTransform(operation.matrix);
		std::visit(DrawVisitor{}, operation.op);
		ctx->SetTransform(oMat);
	}
	operations.clear();
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
	if (!Chakra::VerifyArgCount(argCount, 3, true, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { Chakra::TryGet(arguments, argCount, 3), JsNumber, true}})) return JS_INVALID_REFERENCE;

	auto rect = JsRect::ToRect(arguments[1]);
	auto color = JsColor::ToColor(arguments[2]);
	float radius = 0.f;

	if (auto val = Chakra::TryGet(arguments, argCount, 3)) {
		radius = Chakra::GetNumber(val);
	}

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx) {
		MCDrawUtil dc{thi->cachedCtx, Latite::get().getFont()};

		if (radius > 0.001f) {
			dc.fillRoundedRectangle(rect, color, radius);
		}
		else {
			dc.fillRectangle(rect, color);
		}
		return Chakra::GetUndefined();
	}

	auto mLock = thi->lock();
	thi->operationsDirty.push_back({ thi->matrix, OpFillRect(rect, color, radius) });

	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawRectCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 4, true, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsNumber},  { Chakra::TryGet(arguments, argCount, 4), JsNumber, true} })) return JS_INVALID_REFERENCE;

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
	thi->operationsDirty.push_back({thi->matrix, OpDrawRect(rect, color, thickness) });

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
		dc.drawText(rc, text, color, Renderer::FontSelection::PrimaryRegular, size, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, false);
		dc.flush(true, false);
		return Chakra::GetUndefined();
	}

	auto mLock = thi->lock();
	thi->operationsDirty.push_back({thi->matrix, OpDrawText(rc, text, size, color, Renderer::FontSelection::PrimaryRegular, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR) });
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
		Chakra::ThrowError(XW("Invalid text alignment"));
		return Chakra::GetUndefined();
	}

	auto& ss = SDK::ClientInstance::get()->getGuiData()->screenSize;

	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);
	if (thi->usingMinecraftRend() && thi->cachedCtx) {
		MCDrawUtil dc{ thi->cachedCtx, Latite::get().getFont() };
		dc.drawText(rect, text, color, Renderer::FontSelection::PrimaryRegular, size, (DWRITE_TEXT_ALIGNMENT)align, (DWRITE_PARAGRAPH_ALIGNMENT)vertAlign, false);
		dc.flush(true, false);
		return Chakra::GetUndefined();
	}

	auto thisptr = reinterpret_cast<D2DScriptingObject*>(callbackState);
	auto mLock = thi->lock();
	thisptr->operationsDirty.push_back({ thi->matrix, OpDrawText(rect, text, size, color, Renderer::FontSelection::PrimaryRegular,(DWRITE_TEXT_ALIGNMENT)align, (DWRITE_PARAGRAPH_ALIGNMENT)vertAlign) });
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

	thi->operationsDirty.push_back({ thi->matrix, OpDrawImage{texture, pos, sx, sy, color} });

	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::getTextSize(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3, true)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsString }, { arguments[2], JsNumber } })) return JS_INVALID_REFERENCE;

	auto txt = Chakra::GetString(arguments[1]);
	auto size = Chakra::GetNumber(arguments[2]);
	auto obj = reinterpret_cast<D2DScriptingObject*>(callbackState);

	Vec2 ts;
	if (obj->usingMinecraftRend()) {
		MCDrawUtil dc{ obj->cachedCtx, Latite::get().getFont() };
		ts = dc.getTextSize(txt, Renderer::FontSelection::PrimaryRegular, static_cast<float>(size));
	}
	else {
		D2DUtil dc{};
		ts = dc.getTextSize(txt, Renderer::FontSelection::PrimaryRegular, size, true, false);
	}
	
	return JsScript::getThis()->getClass<JsVec2>()->construct(ts);
}

JsValueRef D2DScriptingObject::setClippingRect(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetUndefined();
	if (!Chakra::VerifyParameters({ {arguments[1], JsObject} })) return JS_INVALID_REFERENCE;

	auto rc = JsRect::ToRect(arguments[1]);
	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);

	if (thi->usingMinecraftRend()) {
		auto guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;
		SDK::RectangleArea rec = { rc.left / guiScale, rc.top / guiScale, rc.right / guiScale, rc.bottom / guiScale };
		thi->cachedCtx->saveCurrentClippingRectangle();
		thi->cachedCtx->setClippingRectangle(rec);
		return Chakra::GetUndefined();
	}

	thi->operationsDirty.push_back({ thi->matrix, OpClip{true, rc} });
	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::restoreClippingRect(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);

	if (thi->usingMinecraftRend()) {
		auto guiScale = SDK::ClientInstance::get()->getGuiData()->guiScale;
		thi->cachedCtx->restoreSavedClippingRectangle();
		return Chakra::GetUndefined();
	}

	thi->operationsDirty.push_back({ thi->matrix, OpClip{false, {}} });
	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::drawTriangle(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return Chakra::GetUndefined();
}

JsValueRef D2DScriptingObject::fillTriangle(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsValueRef();
}

JsValueRef D2DScriptingObject::drawCircle(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsValueRef();
}

JsValueRef D2DScriptingObject::fillCircle(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsValueRef();
}

JsValueRef D2DScriptingObject::drawLine(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsValueRef();
}

JsValueRef D2DScriptingObject::drawItem(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return Chakra::GetUndefined();
	if (!Chakra::VerifyParameters({ {arguments[1], JsObject}, {arguments[2], JsObject}, {arguments[3], JsNumber}, {arguments[4], JsNumber} })) return JS_INVALID_REFERENCE;
	auto thi = reinterpret_cast<D2DScriptingObject*>(callbackState);


	auto item = JsItemStack::Get(arguments[1]);
	if (!item) {
		Chakra::ThrowError(XW("Invalid item"));
		return JS_INVALID_REFERENCE;
	}

	auto pos = JsVec2::ToVec2(arguments[2]);

	if (thi->usingMinecraftRend()) {
		MCDrawUtil dc{ thi->cachedCtx, Latite::get().getFont() };

		dc.drawItem(item, pos, static_cast<float>(Chakra::GetNumber(arguments[3])), static_cast<float>(Chakra::GetNumber(arguments[4])));
		return Chakra::GetUndefined();
	}

	Chakra::ThrowError(XW("Cannot draw items in non-Minecraft renderer mode"));
	return Chakra::GetUndefined();
}

void D2DScriptingObject::DrawVisitor::operator()(OpDrawRect& op) {
	D2DUtil dc;
	dc.drawRectangle(op.rc, op.col, op.thickness);
}

void D2DScriptingObject::DrawVisitor::operator()(OpFillRect& op) {
	D2DUtil dc;

	if (op.radius > 0.01f) {
		dc.fillRoundedRectangle(op.rc, op.col, op.radius);
	}
	else {
		dc.fillRectangle(op.rc, op.col);
	}
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

void D2DScriptingObject::DrawVisitor::operator()(OpClip& op) {
	D2DUtil dc;
	if (op.pushOrPop) {
		dc.ctx->PushAxisAlignedClip(op.rect, D2D1_ANTIALIAS_MODE_ALIASED);
	}
	else {
		dc.ctx->PopAxisAlignedClip();
	}
}
