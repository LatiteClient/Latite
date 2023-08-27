#include "GameScriptingObject.h"
#include "../JsScript.h"
#include "client/script/class/impl/JsVec2.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/world/Minecraft.h"
#include "client/Latite.h"
#include "client/script/ScriptManager.h"
#include "util/Logger.h"
#include "sdk/common/network/packet/TextPacket.h"
#include "util/XorString.h"

void GameScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	this->createWorldObject();
	Chakra::DefineFunc(object, getMousePosCallback, L"getMousePos");
	Chakra::DefineFunc(object, isInUICallback, L"isInUI");
	Chakra::DefineFunc(object, playSoundUI, L"playSoundUI");
	Chakra::DefineFunc(object, sendChatCallback, L"sendChatMessage");
}

void GameScriptingObject::createWorldObject() {
}

JsValueRef GameScriptingObject::getMousePosCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsScript* script;
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JS::JsGetContextData(ctx, reinterpret_cast<void**>(&script));

	auto vec2 = script->findClass<JsVec2>(L"Vector2");
	if (vec2) {
		return vec2->construct(SDK::ClientInstance::get()->cursorPos);
	}

	// could not find Vector2
	__debugbreak();
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::isInUICallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed() ? Chakra::GetTrue() : Chakra::GetFalse();
}

JsValueRef GameScriptingObject::sendChatCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsValueType::JsString} })) return JS_INVALID_REFERENCE;

	JsScript* script;
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JS::JsGetContextData(ctx, reinterpret_cast<void**>(&script));
	
	if (Latite::getScriptManager().hasPermission(script, ScriptManager::Permission::SendChat)) {
		auto lp = SDK::ClientInstance::get()->getLocalPlayer();
		if (lp) {
			SDK::TextPacket tp{};
			String s{};
			s.setString(util::WStrToStr(Chakra::GetString(arguments[1])).c_str());
			tp.chat(s);
			lp->packetSender->sendToServer(tp);
		}

		return Chakra::GetUndefined();
	}
	Chakra::ThrowError(util::StrToWStr(XOR_STRING("Permission denied to use sendChatMessage")));
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::playSoundUI(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsNumber}, {arguments[3], JsNumber} })) return JS_INVALID_REFERENCE;

	std::string soundName = util::WStrToStr(Chakra::GetString(arguments[1]));
	float volume = static_cast<float>(Chakra::GetNumber(arguments[2]));
	float pitch = static_cast<float>(Chakra::GetNumber(arguments[3]));

	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	if (lvl) {
		lvl->playSoundEvent(soundName, SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getOrigin(), pitch);
	}
	return Chakra::GetUndefined();
}
