#pragma once
#include "../ScriptingObject.h"
#include "util/Util.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RenderLayerEvent.h"
#include "client/render/renderer.h"
#include "util/DxUtil.h"
#include <variant>
#include <shared_mutex>
#include <queue>

class D2DScriptingObject : public ScriptingObject {
public:

	inline static int objectID = -1;
	D2DScriptingObject(int id) : ScriptingObject(id, L"graphics") { 
		objectID = id;
		Eventing::get().listen<RenderOverlayEvent>(this, (EventListenerFunc)&D2DScriptingObject::onRenderOverlay, 0);
		Eventing::get().listen<RenderLayerEvent>(this, (EventListenerFunc)&D2DScriptingObject::onRenderLayer, 0);
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

		D2D1::Matrix3x2F matrix;
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
	void onRenderLayer(Event& ev);
	void flushOverlay();

	void setUseMinecraftRend(bool b) { mcRend = b; }
	bool usingMinecraftRend() { return mcRend; }

	[[nodiscard]] D2D1::Matrix3x2F getMatrix() { return matrix; }
	void setMatrix(D2D1::Matrix3x2F const& matr) { matrix = matr; }
private:
	bool mcRend = false;
	D2D1::Matrix3x2F matrix;
	SDK::MinecraftUIRenderContext* cachedCtx = nullptr;

	static JsValueRef CALLBACK useCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK fillRectCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK drawRectCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK drawTextCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK drawTextFullCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	std::shared_mutex mutex;
};