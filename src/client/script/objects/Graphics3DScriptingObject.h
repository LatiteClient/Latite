#pragma once
#include "../ScriptingObject.h"
#include "util/Util.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderLevelEvent.h"
#include "client/render/renderer.h"
#include "util/DxUtil.h"
#include <variant>
#include <shared_mutex>
#include <queue>
#include <sdk/common/client/renderer/Tessellator.h>

class Graphics3DScriptingObject : public ScriptingObject {
public:

	inline static int objectID = -1;
	Graphics3DScriptingObject(int id) : ScriptingObject(id, L"graphics3D") {
		objectID = id;
		Eventing::get().listen<RenderLevelEvent>(this, (EventListenerFunc)&Graphics3DScriptingObject::onRenderLevel, 0);
	}

	~Graphics3DScriptingObject() {
		Eventing::get().unlisten(this);
	}

	void initialize(JsContextRef ctx, JsValueRef parentObj) override;

private:
	void onRenderLevel(Event& ev);

	

	struct Vertex {
		Color color;
		Vec3 position;
	};

	struct DrawCommand {
		bool renderThrough = false;
		SDK::Primitive primitive = SDK::Primitive::Linestrip;
		std::vector<Vertex> vertexBuffer = {};

		DrawCommand() {
			vertexBuffer.reserve(50);
		}

		void reset() {
			renderThrough = false;
			primitive = SDK::Primitive::LineList;
			vertexBuffer.clear();
		}
	};

	SDK::LevelRenderer* levelRenderer = nullptr;
	SDK::ScreenContext* screenContext = nullptr;

	DrawCommand currentCommand{};

	std::vector<DrawCommand> commands = {};

	std::optional<Color> primaryColor = std::nullopt;
	std::array<Color, 4> colors = {};

	static JsValueRef CALLBACK drawLineCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK drawTriangleCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK drawQuadCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK finishCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK setColorCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
	static JsValueRef CALLBACK setColorsCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount, void* callbackState);
};