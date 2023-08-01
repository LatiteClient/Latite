#pragma once
#include "../ScriptingObject.h"
#include "util/Util.h"
#include "util/DxUtil.h"
#include <variant>
#include <shared_mutex>

class D2DScriptingObject : public ScriptingObject {
public:

	inline static int objectID = -1;
	D2DScriptingObject(int id) : ScriptingObject(id, L"d2d") { objectID = id; }

	struct OpDrawRect {
		d2d::Rect rc;
		d2d::Color col;
		float thickness;
	};

	struct OpFillRect {
		d2d::Rect rc;
		d2d::Color col;
	};

	struct DrawOperation {
		std::variant<OpDrawRect, OpFillRect> op;
	};

	std::vector<DrawOperation> operations = {};
	std::shared_lock<std::shared_mutex> lock() {
		return std::shared_lock<std::shared_mutex>(mutex);
	}

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;
private:
	std::shared_mutex mutex;
};