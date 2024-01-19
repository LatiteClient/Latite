#include "pch.h"
#include "ClientScriptingObject.h"
#include "util/Util.h"
#include "util/ChakraUtil.h"
#include "client/Latite.h"
#include "client/script/PluginManager.h"
#include "client/feature/command/CommandManager.h"
#include "client/feature/module/ModuleManager.h"
#include "../class/impl/game/JsItemStack.h"

#include "../class/impl/JsModuleClass.h"
#include "client/feature/command/script/JsCommand.h"

JsValueRef ClientScriptingObject::registerEventCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 3)) return undefined;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString}, {arguments[2], JsFunction} })) return undefined;

	const wchar_t* myS;
	size_t sze;
	JS::JsStringToPointer(arguments[1], &myS, &sze);
	std::wstring wstr(myS);

	for (auto& lis : Latite::getPluginManager().eventListeners) {
		if (lis.first == wstr) {
			JsContextRef ct;
			JS::JsGetCurrentContext(&ct);
			JS::JsAddRef(arguments[2], nullptr);
			lis.second.push_back({ arguments[2], ct });
			return undefined;
		}
	}

	Chakra::ThrowError(L"Invalid event " + wstr);
	return undefined;
}

JsValueRef ClientScriptingObject::runCommandCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetFalse();
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return JS_INVALID_REFERENCE;
	auto s = Chakra::GetString(arguments[1]);
	return Latite::getCommandManager().runCommand(Latite::getCommandManager().prefix + util::WStrToStr(s)) ? Chakra::GetTrue() : Chakra::GetFalse();
}

JsValueRef ClientScriptingObject::showNotifCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 2)) return undefined;
	if (!Chakra::VerifyParameters({ {arguments[1], JsString} })) return undefined;

	// TODO: notifications
	return undefined;
}

JsValueRef ClientScriptingObject::getMmgrCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 1)) return undefined;
	return reinterpret_cast<ClientScriptingObject*>(callbackState)->moduleManager;
}

JsValueRef ClientScriptingObject::getCmgrCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 1)) return undefined;
	return reinterpret_cast<ClientScriptingObject*>(callbackState)->commandManager;
}

JsValueRef ClientScriptingObject::testCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState){
	return JsScript::getThis()->getClass<JsItemStack>()->construct(SDK::ClientInstance::get()->getLocalPlayer()->supplies->inventory->getItem(0), false);
}

void ClientScriptingObject::initModuleManager() {
	JS::JsCreateObject(&this->moduleManager);
	JS::JsAddRef(moduleManager, nullptr);
}

void ClientScriptingObject::initCommandManager() {
	JS::JsCreateObject(&this->commandManager);
	JS::JsAddRef(commandManager, nullptr);
}

void ClientScriptingObject::initialize(JsContextRef ctx, JsValueRef parentObj) {
#if LATITE_DEBUG
	Chakra::DefineFunc(object, testCallback, L"test", this);
#endif

	Chakra::DefineFunc(object, registerEventCallback, L"on");
	Chakra::DefineFunc(object, runCommandCallback, L"runCommand");
	Chakra::DefineFunc(object, showNotifCallback, L"showNotification");
	Chakra::DefineFunc(object, getMmgrCallback, L"getModuleManager", this);
	Chakra::DefineFunc(object, getCmgrCallback, L"getCommandManager", this);

	initModuleManager();
	Chakra::DefineFunc(moduleManager, mmgrRegisterModuleCallback, L"registerModule");
	Chakra::DefineFunc(moduleManager, mmgrDeregisterModuleCallback, L"deregisterModule");
	Chakra::DefineFunc(moduleManager, mmgrGetModuleByName, L"getModuleByName");
	Chakra::DefineFunc(moduleManager, mmgrForEachModule, L"forEachModule");

	initCommandManager();
	Chakra::DefineFunc(commandManager, cmgrRegisterCommandCallback, L"registerCommand");
	Chakra::DefineFunc(commandManager, cmgrDeregisterCommandCallback, L"deregisterCommand");
	Chakra::DefineFunc(commandManager, cmgrGetPrefixCallback, L"getPrefix");

	Chakra::SetPropertyString(object, L"version", util::StrToWStr(std::string(Latite::version.data(), Latite::version.size())));
}


JsValueRef ClientScriptingObject::mmgrRegisterModuleCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 2)) return undefined;

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsObject} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return undefined;
	}

	JsModule* mod = nullptr;

	JS::JsGetExternalData(arguments[1], reinterpret_cast<void**>(&mod));

	if (!mod) {
		Chakra::ThrowError(L"Object is not a module");
		return undefined;
	}

	if (Latite::getModuleManager().registerScriptModule(mod)) {
		JsScript::getThis()->addResource(mod, [](void* obj) {
			if (!Latite::getModuleManager().deregisterScriptModule(reinterpret_cast<JsModule*>(obj))) {
				Logger::Warn("Module is already deregistered");
			}

			// hoepfully this doesnt double delete...
			//delete reinterpret_cast<JsModule*>(obj);
			});
	}

	return undefined;
}

JsValueRef ClientScriptingObject::mmgrDeregisterModuleCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 2)) return undefined;

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsObject} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return undefined;
	}

	JsModule* mod = nullptr;

	JS::JsGetExternalData(arguments[1], reinterpret_cast<void**>(&mod));

	if (!mod || !(mod->getCategory() == Module::SCRIPT)) {
		Chakra::ThrowError(L"Object is not a valid script module");
		return undefined;
	}

	//Latite::getModuleManager().deregisterScriptModule(mod);
	JsScript::getThis()->removeResource(mod); // this should deregister it I think

	return undefined;
}

JsValueRef ClientScriptingObject::mmgrGetModuleByName(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsValueRef null;
	JS::JsGetNullValue(&null);

	if (!Chakra::VerifyArgCount(argCount, 2)) return null;

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsString} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return null;
	}

	std::string str = util::WStrToStr(Chakra::GetString(arguments[1]));

	auto thi = reinterpret_cast<ClientScriptingObject*>(callbackState);
	auto mod = Latite::getModuleManager().find(str);

	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JsScript* script = JsScript::getThis();
	if (script && mod) {
		auto cl = script->getClass<JsModuleClass>();
		auto r = cl->construct(reinterpret_cast<JsModule*>(mod.get()), false);
		return r;
	}

	return null;
}

JsValueRef ClientScriptingObject::mmgrForEachModule(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState)
{
	JsValueRef null;
	JS::JsGetUndefinedValue(&null);

	if (!Chakra::VerifyArgCount(argCount, 2)) return null;

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsFunction} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return null;
	}

	auto thi = reinterpret_cast<ClientScriptingObject*>(callbackState);
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JsScript* script = JsScript::getThis();
	
	auto cl = script->getClass<JsModuleClass>();
	if (!cl) {
		Chakra::ThrowError(L"INTERNAL ERROR: could not find Module class");
		return JS_INVALID_REFERENCE;
	}

	Latite::getModuleManager().forEach([&](std::shared_ptr<IModule> modul) {
		JsValueRef r[2] = { arguments[0], cl->construct(reinterpret_cast<JsModule*>(modul.get()), false) };
		JsValueRef res;
		Latite::getPluginManager().handleErrors(Chakra::CallFunction(arguments[1], r, 2, &res));
		Chakra::Release(res);
		Chakra::Release(r[1]);
		return false;
		});

	return null;
}

JsValueRef ClientScriptingObject::cmgrRegisterCommandCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 2)) return undefined;

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsObject} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return undefined;
	}

	JsCommand* cmd;

	JS::JsGetExternalData(arguments[1], reinterpret_cast<void**>(&cmd));

	if (!cmd) {
		Chakra::ThrowError(L"Object is not a command");
		return undefined;
	}

	if (Latite::getCommandManager().registerScriptCommand(cmd)) {
		JsScript::getThis()->addResource(cmd, [](void* obj) {
			if (!Latite::getCommandManager().deregisterScriptCommand(reinterpret_cast<JsCommand*>(obj))) {
				Logger::Warn("Script is already deregistered");
			}

			// hoepfully this doesnt double delete...
			//delete reinterpret_cast<JsCommand*>(obj);
			});
	}
	return undefined;
}

JsValueRef ClientScriptingObject::cmgrDeregisterCommandCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState) {
	JsValueRef undefined;
	JS::JsGetUndefinedValue(&undefined);
	if (!Chakra::VerifyArgCount(argCount, 2)) return undefined;

	auto sz = Chakra::VerifyParameters({ {arguments[1], JsObject} });
	if (!sz.success) {
		// handle
		Chakra::ThrowError(sz.str);
		return undefined;
	}

	JsCommand* cmd;

	// TODO: check if its a script command

	JS::JsGetExternalData(arguments[1], reinterpret_cast<void**>(&cmd));

	//Latite::getCommandManager().deregisterScriptCommand(cmd);
	JsScript::getThis()->removeResource(cmd);

	return undefined;
}

JsValueRef ClientScriptingObject::cmgrGetPrefixCallback(JsValueRef callee, bool isConstructor, JsValueRef* arguments, unsigned short argCount, void* callbackState){
	return Chakra::MakeString(util::StrToWStr(Latite::getCommandManager().prefix));
}
