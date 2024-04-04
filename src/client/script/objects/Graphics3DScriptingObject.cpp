#include "pch.h"
#include "Graphics3DScriptingObject.h"
#include <sdk/common/client/renderer/MeshUtils.h>
#include <sdk/common/client/renderer/MaterialPtr.h>
#include <client/script/class/impl/JsColor.h>
#include <client/script/class/impl/JsVec3.h>

void Graphics3DScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, drawLineCallback, L"drawLine", this);
	Chakra::DefineFunc(object, drawTriangleCallback, L"drawTriangle", this);
	Chakra::DefineFunc(object, drawQuadCallback, L"drawQuad", this);
	Chakra::DefineFunc(object, finishCallback, L"finish", this);
}

void Graphics3DScriptingObject::onRenderLevel(Event& evG) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evG);

	screenContext = ev.getScreenContext();
	levelRenderer = ev.getLevelRenderer();

	for (auto& command : commands) {
		auto tess = screenContext->tess;
		tess->begin(command.primitive, command.vertexBuffer.size());
		
		for (auto& vertex : command.vertexBuffer) {
			tess->color(vertex.color);
			tess->vertex(vertex.position.x, vertex.position.y, vertex.position.z);
		}

		SDK::MeshHelpers::renderMeshImmediately(screenContext, tess, command.renderThrough ? levelRenderer->getLevelRendererPlayer()->getSelectionBoxMaterial() : SDK::MaterialPtr::getUIColor());
	}

	commands.clear();
}

JsValueRef Graphics3DScriptingObject::drawLineCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsObject }})) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->currentCommand.primitive = SDK::Primitive::Linestrip;

	auto col = JsColor::ToColor(arguments[3]);
	auto p1 = JsVec3::ToVec3(arguments[1]);
	auto p2 = JsVec3::ToVec3(arguments[2]);

	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p1 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p2 });

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::drawTriangleCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsObject }, { arguments[4], JsObject } })) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->currentCommand.primitive = SDK::Primitive::Trianglestrip;

	auto col = JsColor::ToColor(arguments[4]);
	auto p1 = JsVec3::ToVec3(arguments[1]);
	auto p2 = JsVec3::ToVec3(arguments[2]);
	auto p3 = JsVec3::ToVec3(arguments[3]);

	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p1 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p2 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p3 });

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::drawQuadCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 6)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsObject }, { arguments[4], JsObject } })) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->currentCommand.primitive = SDK::Primitive::Quad;

	auto col = JsColor::ToColor(arguments[5]);
	auto p1 = JsVec3::ToVec3(arguments[1]);
	auto p2 = JsVec3::ToVec3(arguments[2]);
	auto p3 = JsVec3::ToVec3(arguments[3]);
	auto p4 = JsVec3::ToVec3(arguments[4]);

	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p1 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p2 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p3 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ col, p4 });

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::finishCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyParameters({ { Chakra::TryGet(arguments, argCount, 1), JsNumber, true }})) return JS_INVALID_REFERENCE;
	
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->commands.push_back(obj->currentCommand);

	return Chakra::GetUndefined();
}