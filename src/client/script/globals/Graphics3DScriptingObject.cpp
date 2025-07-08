#include "pch.h"
#include "Graphics3DScriptingObject.h"
#include <mc/common/client/renderer/MeshUtils.h>
#include <mc/common/client/renderer/MaterialPtr.h>
#include <client/script/class/classes/JsColor.h>
#include <client/script/class/classes/JsVec3.h>

void Graphics3DScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	Chakra::DefineFunc(object, setColorCallback, XW("setColor"), this);
	Chakra::DefineFunc(object, setColorsCallback, XW("setColors"), this);
	Chakra::DefineFunc(object, drawLineCallback, XW("drawLine"), this);
	Chakra::DefineFunc(object, drawTriangleCallback, XW("drawTriangle"), this);
	Chakra::DefineFunc(object, drawQuadCallback, XW("drawQuad"), this);
	Chakra::DefineFunc(object, finishCallback, XW("finish"), this);
}

void Graphics3DScriptingObject::onRenderLevel(Event& evG) {
	auto& ev = reinterpret_cast<RenderLevelEvent&>(evG);

	screenContext = ev.getScreenContext();
	levelRenderer = ev.getLevelRenderer();

	*screenContext->shaderColor = { 1.f, 1.f, 1.f, 1.f };

	for (auto& command : commands) {
		auto tess = screenContext->tess;
		tess->begin(command.primitive, command.vertexBuffer.size());
		auto origin = levelRenderer->getLevelRendererPlayer()->getOrigin();

		for (auto& vertex : command.vertexBuffer) {
			tess->color(vertex.color);
			tess->vertex(vertex.position.x - origin.x, vertex.position.y - origin.y, vertex.position.z - origin.z);
		}

		SDK::MeshHelpers::renderMeshImmediately(screenContext, tess, command.renderThrough ? SDK::MaterialPtr::getSelectionBoxMaterial() : SDK::MaterialPtr::getUIColor());
	}

	commands.clear();
}

JsValueRef Graphics3DScriptingObject::drawLineCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 3)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }})) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->currentCommand.primitive = SDK::Primitive::LineList;

	auto p1 = JsVec3::ToVec3(arguments[1]);
	auto p2 = JsVec3::ToVec3(arguments[2]);

	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[0]), p1 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[1]), p2 });

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::drawTriangleCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsObject } })) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->currentCommand.primitive = SDK::Primitive::Trianglestrip;

	auto p1 = JsVec3::ToVec3(arguments[1]);
	auto p2 = JsVec3::ToVec3(arguments[2]);
	auto p3 = JsVec3::ToVec3(arguments[3]);

	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[0]), p1 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[1]), p2 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[2]), p3 });

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::drawQuadCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject }, { arguments[2], JsObject }, { arguments[3], JsObject } })) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	obj->currentCommand.primitive = SDK::Primitive::Quad;

	auto p1 = JsVec3::ToVec3(arguments[1]);
	auto p2 = JsVec3::ToVec3(arguments[2]);
	auto p3 = JsVec3::ToVec3(arguments[3]);
	auto p4 = JsVec3::ToVec3(arguments[4]);

	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[0]), p1 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[1]), p2 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[2]), p3 });
	obj->currentCommand.vertexBuffer.push_back(Vertex{ obj->primaryColor.value_or(obj->colors[3]), p4 });

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::finishCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyParameters({ { Chakra::TryGet(arguments, argCount, 1), JsBoolean, true }})) return JS_INVALID_REFERENCE;
	
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);
	auto val = Chakra::TryGet(arguments, argCount, 1);

	if (val) {
		obj->currentCommand.renderThrough = !Chakra::GetBool(val);
	}
	obj->commands.push_back(obj->currentCommand);
	obj->currentCommand.reset();

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::setColorCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject } })) return JS_INVALID_REFERENCE;
	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);

	auto col = JsColor::ToColor(arguments[1]);
	obj->primaryColor = col;

	return Chakra::GetUndefined();
}

JsValueRef Graphics3DScriptingObject::setColorsCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 5)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ { arguments[1], JsObject } })) return JS_INVALID_REFERENCE;

	auto obj = reinterpret_cast<Graphics3DScriptingObject*>(callbackState);

	auto col = JsColor::ToColor(arguments[1]);
	auto col1 = JsColor::ToColor(arguments[2]);
	auto col2 = JsColor::ToColor(arguments[3]);
	auto col3 = JsColor::ToColor(arguments[4]);
	obj->primaryColor = std::nullopt;
	obj->colors = { col, col1, col2, col3 };

	return Chakra::GetUndefined();
}
