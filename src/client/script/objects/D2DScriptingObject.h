/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../ScriptingObject.h"
#include "util/Util.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/render/renderer.h"
#include "util/DxUtil.h"
#include <variant>
#include <shared_mutex>
#include <queue>

class D2DScriptingObject : public ScriptingObject {
public:

	inline static int objectID = -1;
	D2DScriptingObject(int id) : ScriptingObject(id, L"dx") { 
		objectID = id;
		Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&D2DScriptingObject::onRenderOverlay, 0);
	}

	~D2DScriptingObject() {
		Eventing::get().unlisten(this);
	}

	

	struct OpDrawRect {
		d2d::Rect rc;
		d2d::Color col;
		float thickness;
	};

	struct OpFillRect {
		d2d::Rect rc;
		d2d::Color col;

		OpFillRect(const d2d::Rect& rc, const d2d::Color& col)
			: rc(rc), col(col) {
		}
	};

	struct OpDrawText {
		d2d::Rect rect;
		std::wstring text;
		float size;
		d2d::Color col;
		Renderer::FontSelection font;
		DWRITE_TEXT_ALIGNMENT alignment;
		DWRITE_PARAGRAPH_ALIGNMENT vertAlign;


		

		OpDrawText(const d2d::Rect& rect, const std::wstring& text, float size, const d2d::Color& col, const Renderer::FontSelection& font, const DWRITE_TEXT_ALIGNMENT& alignment, const DWRITE_PARAGRAPH_ALIGNMENT& vertAlign)
			: rect(rect), text(text), size(size), col(col), font(font), alignment(alignment), vertAlign(vertAlign)
		{
		}
	};

	struct DrawOperation {
		enum Type {
			DrawRect = 0,
			FillRect
		};

		std::variant<OpDrawRect, OpFillRect, OpDrawText> op;
	};

	struct DrawVisitor {
		void operator()(OpDrawRect& op);
		void operator()(OpFillRect& op);
		void operator()(OpDrawText& op);
	};

	std::queue<DrawOperation> operations = {};
	std::shared_lock<std::shared_mutex> lock() {
		return std::shared_lock<std::shared_mutex>(mutex);
	}

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

	void onRenderOverlay(Event& ev);
private:
	static JsValueRef CALLBACK fillRectCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK drawRectCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	std::shared_mutex mutex;
};