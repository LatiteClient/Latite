#pragma once
#include "../ScriptingObject.h"
#include "util/Util.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
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

	struct DrawOperation {
		enum Type {
			DrawRect = 0,
			FillRect
		};

		std::variant<OpDrawRect, OpFillRect> op;
	};

	struct DrawVisitor {
		void operator()(OpDrawRect& op);
		void operator()(OpFillRect& op);
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