#include "pch.h"
#include "GameScriptingObject.h"
#include "../JsScript.h"
#include "client/script/class/impl/JsVec2.h"
#include "sdk/common/client/game/ClientInstance.h"
#include "sdk/common/client/game/MinecraftGame.h"
#include "sdk/common/client/renderer/game/LevelRenderer.h"
#include "sdk/common/client/player/LocalPlayer.h"
#include "sdk/common/world/Minecraft.h"
#include "sdk/common/world/level/BlockSource.h"

#include "client/Latite.h"
#include <client/input/Keyboard.h>
#include "client/script/PluginManager.h"
#include "util/Logger.h"
#include "sdk/common/network/packet/TextPacket.h"
#include "util/XorString.h"
#include "../class/impl/game/JsEntityClass.h"
#include "../class/impl/game/JsPlayerClass.h"
#include "../class/impl/game/JsLocalPlayerClass.h"
#include <sdk/common/network/packet/CommandRequestPacket.h>
#include <client/script/class/impl/game/JsBlock.h>
#include <client/script/class/impl/JsVec3.h>

void GameScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
	this->createWorldObject();
	Chakra::SetPropertyString(object, XW("version"), util::StrToWStr(Latite::get().gameVersion));
	Chakra::DefineFunc(object, getLocalPlayerCallback, XW("getLocalPlayer"));
	Chakra::DefineFunc(object, getMousePosCallback, XW("getMousePos"));
	Chakra::DefineFunc(object, getScreenSizeCallback, XW("getScreenSize"));
	Chakra::DefineFunc(object, isInUICallback, XW("isInUI"));
	Chakra::DefineFunc(object, playSoundUI, XW("playSoundUI"));
	Chakra::DefineFunc(object, sendChatCallback, XW("sendChatMessage"));
	Chakra::DefineFunc(object, executeCommand, XW("executeCommand"));
	Chakra::DefineFunc(object, getWorldCallback, XW("getWorld"), this);
	Chakra::DefineFunc(object, getDimensionCallback, XW("getDimension"), this);
	Chakra::DefineFunc(object, getConnectedServerCallback, XW("getServer"));
	Chakra::DefineFunc(object, getPortCallback, XW("getPort"));
	Chakra::DefineFunc(object, getConnectedFeaturedServerCallback, XW("getFeaturedServer"));
	Chakra::DefineFunc(object, getInputBinding, XW("getInputBinding"));
	Chakra::DefineFunc(object, getFOV, XW("getFOV"));
	Chakra::DefineFunc(object, getCameraPosition, XW("getCameraPosition"));
	Chakra::DefineFunc(object, captureCursor, XW("captureCursor"));
	Chakra::DefineFunc(object, releaseCursor, XW("releaseCursor"));
}

void GameScriptingObject::createWorldObject() {
	JS::JsCreateObject(&worldObj);
	//JS::JsAddRef(worldObj, nullptr); // never forget to add a reference to an object that isn't set !!

	JS::JsCreateObject(&dimensionObj);
	//JS::JsAddRef(dimensionObj, nullptr);

	Chakra::DefineFunc(worldObj, worldExists, XW("exists"), this);
	Chakra::DefineFunc(worldObj, worldGetEntList, XW("getEntities"), this);
	Chakra::DefineFunc(worldObj, worldGetEntCount, XW("getEntityCount"), this);
	Chakra::DefineFunc(worldObj, worldGetPlayers, XW("getPlayers"), this);
	Chakra::DefineFunc(worldObj, worldGetName, XW("getName"), this);
	Chakra::DefineFunc(dimensionObj, worldExists, XW("exists"), this);
	Chakra::DefineFunc(dimensionObj, dimensionGetBlock, XW("getBlock"), this);
	Chakra::DefineFunc(dimensionObj, dimensionGetName, XW("getName"), this);
	Chakra::SetProperty(Chakra::GetGlobalObject(), XW("world"), worldObj);
	Chakra::SetProperty(Chakra::GetGlobalObject(), XW("dimension"), dimensionObj);
}

JsValueRef GameScriptingObject::worldExists(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return SDK::ClientInstance::get()->minecraft->getLevel() ? Chakra::GetTrue() : Chakra::GetFalse();
}

JsValueRef GameScriptingObject::worldGetName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(XW("World is not allowed to be used here"));
		return Chakra::GetUndefined();
	}

	return Chakra::MakeString(util::StrToWStr(SDK::ClientInstance::get()->minecraft->getLevel()->name));
}

JsValueRef GameScriptingObject::worldGetPlayers(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(XW("World is not allowed to be used here"));
		return Chakra::GetUndefined();
	}

	JsValueRef array;
	auto arr = SDK::ClientInstance::get()->minecraft->getLevel()->getPlayerList();
	auto size = arr->size();
	JS::JsCreateArray(static_cast<unsigned>(size), &array);

	int i = 0;
	for (auto& pair : *arr) {
		JS::JsSetIndexedProperty(array, Chakra::MakeInt(i), Chakra::MakeString(util::StrToWStr(pair.second.name)));
		++i;
	}
	return array;

}

JsValueRef GameScriptingObject::worldGetEntList(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(L"World is not available");
		return Chakra::GetUndefined();
	}

	JsPlugin* scr = JsScript::getThis()->getPlugin();

	if (!Latite::getPluginManager().hasPermission(scr, PluginManager::Permission::Operator)) {
		Chakra::ThrowError(util::StrToWStr(XOR_STRING("No permission to use getEntityList here")));
		return JS_INVALID_REFERENCE;
	}

	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	JsValueRef array;
	auto entList = lvl->getRuntimeActorList();
	unsigned sz = static_cast<unsigned>(entList.size());
	JS::JsCreateArray(sz, &array);
	unsigned idx = 0;
	for (auto& ent : entList) {
		JsValueRef db;
		JS::JsDoubleToNumber(static_cast<double>(idx), &db);
		JsScript* script = JsScript::getThis();

		auto entc = script->getClass<JsEntityClass>();
		auto plrc = script->getClass<JsPlayerClass>();
		auto lplrc = script->getClass<JsLocalPlayerClass>();

		if (ent->getRuntimeID() == 1) {
			JS::JsSetIndexedProperty(array, db, lplrc->construct(new JsEntity(ent->getRuntimeID(), JsEntity::AccessLevel::LocalPlayer), true));
		} else if (ent->isPlayer()) {
			JS::JsSetIndexedProperty(array, db, plrc->construct(new JsEntity(ent->getRuntimeID()), true));
		} else JS::JsSetIndexedProperty(array, db, entc->construct(new JsEntity(ent->getRuntimeID()), true));
		idx++;
		Chakra::Release(db);
	}
	return array;
}

JsValueRef GameScriptingObject::worldGetEntCount(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(XW("World is not available"));
		return Chakra::GetUndefined();
	}

	JsScript* scr = JsScript::getThis();

	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	auto entList = lvl->getRuntimeActorList();
	return Chakra::MakeInt(static_cast<int>(entList.size()));
}

JsValueRef GameScriptingObject::dimensionGetName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(XW("Dimension is not available"));
		return Chakra::GetUndefined();
	}

	auto& dim = SDK::ClientInstance::get()->getLocalPlayer()->dimension;
	return Chakra::MakeString(util::StrToWStr(dim->dimensionName));
}

JsValueRef GameScriptingObject::dimensionGetBlock(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsValueType::JsNumber}, {arguments[2], JsValueType::JsNumber}, {arguments[3], JsValueType::JsNumber} })) return JS_INVALID_REFERENCE;
	
	int x = Chakra::GetInt(arguments[1]);
	int y = Chakra::GetInt(arguments[2]);
	int z = Chakra::GetInt(arguments[3]);

	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		Chakra::ThrowError(XW("Dimension is not available"));
		return Chakra::GetUndefined();
	}

	JsScript* scr = JsScript::getThis();
	if (!Latite::getPluginManager().hasPermission(scr->getPlugin(), PluginManager::Permission::Operator)) {
		Chakra::ThrowError(util::StrToWStr(XOR_STRING("No permission to use getBlock here")));
		return JS_INVALID_REFERENCE;
	}

	auto reg = SDK::ClientInstance::get()->getRegion();
	if (!reg) {
		return Chakra::GetNull();
	}

	auto block = reg->getBlock(x, y, z);

	if (!block) {
		return Chakra::GetNull();
	}
	
	return scr->getClass<JsBlock>()->construct(block, false);
}

JsValueRef GameScriptingObject::getMousePosCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsScript* script = JsScript::getThis();

	auto vec2 = script->getClass<JsVec2>();
	if (vec2) {
		return vec2->construct(SDK::ClientInstance::get()->cursorPos);
	}

	// could not find Vector2
	__debugbreak();
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::getScreenSizeCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return JsScript::getThis()->getClass<JsVec2>()->construct(SDK::ClientInstance::get()->getGuiData()->screenSize);
}

JsValueRef GameScriptingObject::getInputBinding(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto handler = SDK::ClientInstance::get()->inputHandler;
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsValueType::JsString} })) return JS_INVALID_REFERENCE;


	auto key = Latite::getKeyboard().getMappedKey(util::WStrToStr(Chakra::GetString(arguments[1])));
	return Chakra::MakeInt(key);
}

JsValueRef GameScriptingObject::getWorldCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	//if (!SDK::ClientInstance::get()->minecraft->getLevel()) return Chakra::GetNull();
	JsContextRef ct;
	JS::JsGetCurrentContext(&ct);
	return reinterpret_cast<GameScriptingObject*>(callbackState)->worldObj;
}

JsValueRef GameScriptingObject::getDimensionCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	//if (!SDK::ClientInstance::get()->minecraft->getLevel()) return Chakra::GetNull();
	JsContextRef ct;
	JS::JsGetCurrentContext(&ct);
	return reinterpret_cast<GameScriptingObject*>(callbackState)->dimensionObj;
}

JsValueRef GameScriptingObject::isInUICallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	return SDK::ClientInstance::get()->minecraftGame->isCursorGrabbed() ? Chakra::GetFalse() : Chakra::GetTrue();
}

JsValueRef GameScriptingObject::getFOV(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		return Chakra::GetNull();
	}

	auto lrp = SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer();
	return JsScript::getThis()->getClass<JsVec2>()->construct(Vec2(lrp->getFovX(), lrp->getFovY()));
}

JsValueRef GameScriptingObject::getCameraPosition(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!SDK::ClientInstance::get()->getLocalPlayer()) {
		return Chakra::GetNull();
	}

	auto lrp = SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer();
	return JsScript::getThis()->getClass<JsVec3>()->construct(lrp->getOrigin());
}

JsValueRef GameScriptingObject::captureCursor(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	SDK::ClientInstance::get()->grabCursor();
	return Chakra::GetNull();
}

JsValueRef GameScriptingObject::releaseCursor(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {	
	SDK::ClientInstance::get()->releaseCursor();
	return Chakra::GetNull();
}

JsValueRef GameScriptingObject::sendChatCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsValueType::JsString} })) return JS_INVALID_REFERENCE;

	JsPlugin* script = JsScript::getThis()->getPlugin();
	
	if (Latite::getPluginManager().hasPermission(script, PluginManager::Permission::SendChat)
		&& Chakra::GetString(arguments[1]).size() < 250) {
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

JsValueRef GameScriptingObject::executeCommand(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetUndefined();
	if (!Chakra::VerifyParameters({ {arguments[1], JsValueType::JsString} })) return JS_INVALID_REFERENCE;
	auto str = util::WStrToStr(Chakra::GetString(arguments[1]));

	if (str.size() > 250 || str.size() < 1 || str.at(0) != '/')
		return Chakra::GetUndefined();

	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (lp) {
		auto str = util::WStrToStr(Chakra::GetString(arguments[1]));
		SDK::CommandRequestPacket req{str};
		lp->packetSender->sendToServer(req);
	}
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::playSoundUI(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	if (!Chakra::VerifyArgCount(argCount, 4)) return JS_INVALID_REFERENCE;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsNumber}, {arguments[3], JsNumber} })) return JS_INVALID_REFERENCE;

	std::string soundName = util::WStrToStr(Chakra::GetString(arguments[1]));
	float volume = static_cast<float>(Chakra::GetNumber(arguments[2]));
	float pitch = static_cast<float>(Chakra::GetNumber(arguments[3]));

	auto lvl = SDK::ClientInstance::get()->minecraft->getLevel();
	if (lvl && SDK::ClientInstance::get()->levelRenderer) {
		lvl->playSoundEvent(soundName, SDK::ClientInstance::get()->levelRenderer->getLevelRendererPlayer()->getOrigin(), volume, pitch);
	}
	return Chakra::GetUndefined();
}

JsValueRef GameScriptingObject::getLocalPlayerCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto lp = SDK::ClientInstance::get()->getLocalPlayer();
	if (!lp) return Chakra::GetNull();
	JsScript* script = JsScript::getThis();

	auto cl = script->getClass<JsLocalPlayerClass>();
	return cl->construct(new JsEntity(1, JsEntity::AccessLevel::LocalPlayer), true);
}

JsValueRef GameScriptingObject::getConnectedServerCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto rak = SDK::RakNetConnector::get();
	if (!rak) return Chakra::GetNull();
	if (rak->ipAddress.size() == 0) return Chakra::GetNull();

	std::wstring conn = util::StrToWStr(rak->ipAddress);
	JsValueRef ret;
	JS::JsPointerToString(conn.c_str(), conn.size(), &ret);
	return ret;
}

JsValueRef GameScriptingObject::getConnectedFeaturedServerCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto rak = SDK::RakNetConnector::get();
	if (!rak) return Chakra::GetNull();
	if (rak->featuredServer.size() == 0) return Chakra::GetNull();

	std::wstring conn = util::StrToWStr(rak->featuredServer);
	JsValueRef ret;
	JS::JsPointerToString(conn.c_str(), conn.size(), &ret);
	return ret;
}

JsValueRef GameScriptingObject::getPortCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	auto rak = SDK::RakNetConnector::get();
	if (!rak) return Chakra::MakeInt(0);

	return Chakra::MakeInt(rak->port);
}
