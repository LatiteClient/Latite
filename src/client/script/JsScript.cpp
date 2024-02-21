#include "pch.h"
#include "JsScript.h"

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/windows.security.cryptography.h>
#include <winrt/windows.security.cryptography.core.h>

#include "Lib/Libraries/Filesystem.h"
#include "Lib/Libraries/Network.h"
#include "Lib/Libraries/Clipboard.h"

#include "objects/ClientScriptingObject.h"

#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"

#include "class/impl/JsVec2.h"
#include "class/impl/JsVec3.h"
#include "class/impl/JsRect.h"
#include "class/impl/JsColor.h"
#include "class/impl/JsModuleClass.h"
#include "class/impl/JsHudModuleClass.h"
#include "class/impl/JsTextModuleClass.h"
#include "class/impl/JsTextureClass.h"
#include "class/impl/JsSettingClass.h"
#include "class/impl/JsCommandClass.h"
#include "class/impl/JsNativeModule.h"
#include "class/impl/game/JsEntityClass.h"
#include "class/impl/game/JsPlayerClass.h"
#include "class/impl/game/JsLocalPlayerClass.h"
#include "class/impl/game/JsItem.h"
#include "class/impl/game/JsItemStack.h"

#include "objects/GameScriptingObject.h"
#include "objects/D2DScriptingObject.h"
#include "util/XorString.h"
#include "util/Logger.h"

#include <resource.h>

using namespace winrt::Windows::Web::Http;


bool JsScript::load() {
	auto res = JS::JsCreateContext(plugin->getRuntime(), &this->ctx) == JsNoError;
	if (!res) return false;

	data.name = this->getPlugin()->getName();
	data.author = this->getPlugin()->getAuthor();
	data.version = this->getPlugin()->getVersion();
	data.description = this->getPlugin()->getDescription();

	loadScriptObjects();
	loadJSApi();

	stream.open(getPath());
	if (stream.fail()) {
		Logger::Warn("Could not open script path {}: {}", getPath().string(), errno);
		return false;
	}

	std::wstringstream buffer;
	buffer << stream.rdbuf();
	this->loadedScript = /*L"\"use strict\";" + */buffer.str();
	this->stream.close();

	this->libraries.push_back(std::make_shared<Filesystem>(this));
	this->libraries.push_back(std::make_shared<Network>(this));
	this->libraries.push_back(std::make_shared<Clipboard>(this));

	JS::JsSetContextData(ctx, this);
	JS::JsSetCurrentContext(ctx);

	if (GetModuleHandleA("Chakra.dll")) {
		Chakra::pass("JsStartDebugging")();
	}

	return res;
}

JsValueRef JsScript::getModuleExports() {

	JsContextRef oCtx;
	JS::JsGetCurrentContext(&oCtx);

	JS::JsSetCurrentContext(this->ctx);
	auto global = Chakra::GetGlobalObject();
	auto objectClass = Chakra::GetProperty(global, L"Object");
	auto keysFunc = Chakra::GetProperty(objectClass, L"keys");

	auto exp = Chakra::GetProperty(global, L"exports");
	JS::JsSetCurrentContext(oCtx);

	if (exp != JS_INVALID_REFERENCE) {
		JsValueRef result;
		JsValueRef args[] = {objectClass, exp};
		Chakra::CallFunction(keysFunc, args, 2, &result);

		auto length = Chakra::GetInt(Chakra::GetProperty(result, L"length"));
		Chakra::Release(result);

		if (length > 0) {
			return exp;
		}
	}

	auto mod = Chakra::GetProperty(global, L"module");
	if (mod != JS_INVALID_REFERENCE) {
		return Chakra::GetProperty(mod, L"exports");
	}
	return JS_INVALID_REFERENCE;
}

JsScript::JsScript(JsPlugin* plugin, std::filesystem::path const& path, std::wstring const& relPath)
	: plugin(plugin), path(path), relPath(relPath) {
	this->relFolderPath = plugin->getRelFolderPath();
}


void JsScript::handleAsyncOperations() {
	for (size_t i = 0; i < pendingOperations.size();) {
		auto ptr = pendingOperations[i];
		if (ptr->flagDone) {
			ptr->getArgs();
			ptr->call();
			if (ptr->shouldRemove) {
				pendingOperations.erase(pendingOperations.begin() + i);
				continue;
			}
		}
		++i;
	}
	return;


	for (auto it = pendingOperations.begin(); it != pendingOperations.end();) {
		auto ptr = it->get();
		if (ptr->flagDone) {
			ptr->thr->join();
			ptr->getArgs();
			ptr->call();
			if (ptr->shouldRemove) {
				pendingOperations.erase(it);
				continue;
			}
		}
		++it;
	}
}

JsErrorCode JsScript::runScript() {
	JS::JsSetCurrentContext(ctx);
	//this->checkTrusted();
#if LATITE_DEBUG
	//Logger::Info("isTrusted = {}", this->isTrusted());
#endif

	JsErrorCode code = JsNoError;
	try {
		 code = JS::JsRunScript(loadedScript.c_str(), util::fnv1a_32(util::WStrToStr(this->path.wstring())), this->path.wstring().c_str(), nullptr);
	}
	catch (...) {
		//Latite::getClientMessageSink().push("Exception while loading script " + path.string() + ": " + e.what());
	}
	return code;
}

JsErrorCode JsScript::compileScript() {
	constexpr size_t cBufSize = 32768;
	unsigned int bufferSize = cBufSize;
	BYTE* scriptBuf = new BYTE[cBufSize];

	auto err = JS::JsSerializeScript(loadedScript.c_str(), scriptBuf, &bufferSize);

	auto objFolder = this->getPlugin()->getPath() / "obj";
	std::filesystem::create_directory(objFolder);

	auto objPath = objFolder / (this->relPath.substr(0, relPath.size() - 3) /*remove .js*/ + L".obj");

	std::ofstream ofs{objPath};
	for (int i = 0; i < bufferSize; ++i) {
		ofs << (char)scriptBuf[i];
	}

	delete[] scriptBuf;
	return err;
}

void JsScript::loadJSApi() {
	JS::JsSetCurrentContext(ctx);
	JsValueRef res;
	auto err = JS::JsRunScript(util::StrToWStr(Latite::get().getTextAsset(JS_LATITEAPI)).c_str(), sCtx, L"latiteapi.js", &res);
	Latite::getPluginManager().handleErrors(err);
	if (!err) {
		JS::JsRelease(res, nullptr);
	}
}

JsScript* JsScript::getThis() {
	JsScript* ret = nullptr;
	JsContextRef ct;
	JS::JsGetCurrentContext(&ct);
	JS::JsGetContextData(ct, reinterpret_cast<void**>(&ret));
	return ret;
}


namespace {
	JsValueRef CALLBACK setTimeoutCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState) {

		if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();
		auto sz = Chakra::VerifyParameters({ {arguments[1], JsValueType::JsFunction}, {arguments[2], JsValueType::JsNumber} });
		if (!sz) {
			Chakra::ThrowError(sz.str);
			return Chakra::GetUndefined();
		}

		auto func = arguments[1];
		auto num = Chakra::GetNumber(arguments[2]);

		JsScript* thi = reinterpret_cast<JsScript*>(callbackState);
		auto& tim = thi->timeouts.emplace_back(static_cast<int>(thi->timeouts.size() + 1), static_cast<long long>(num), func);
		JsValueRef ret;
		JS::JsIntToNumber(tim.id, &ret);
		return ret;
	}

	JsValueRef CALLBACK setIntervalCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState) {
		if (!Chakra::VerifyArgCount(argCount, 3)) return Chakra::GetUndefined();
		auto sz = Chakra::VerifyParameters({ {arguments[1], JsValueType::JsFunction}, {arguments[2], JsValueType::JsNumber} });
		if (!sz) {
			Chakra::ThrowError(sz.str);
			return Chakra::GetUndefined();
		}

		auto func = arguments[1];
		auto num = Chakra::GetNumber(arguments[2]);

		JsScript* thi = reinterpret_cast<JsScript*>(callbackState);
		auto& tim = thi->intervals.emplace_back(static_cast<int>(thi->intervals.size() + 1), static_cast<long long>(num), func);
		JsValueRef ret;
		JS::JsIntToNumber(tim.id, &ret);
		return ret;
	}

	JsValueRef CALLBACK sleepCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState) {
		if (!Chakra::VerifyArgCount(argCount, 2)) return Chakra::GetUndefined();
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(Chakra::GetNumber(arguments[1]))));
		return Chakra::GetUndefined();
	}
	JsValueRef CALLBACK clientMessageCallback(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState) {

		JsValueRef undef;

		JS::JsGetUndefinedValue(&undef);

		for (unsigned short arg = 1; arg < argCount; arg++) {
			auto myRef = arguments[arg];

			JsValueRef myString;
			JS::JsConvertValueToString(myRef, &myString);

			const wchar_t* result;
			size_t len;
			JS::JsStringToPointer(myString, &result, &len);

			std::string fs = util::WStrToStr(result);
			Latite::getClientMessageSink().display(fs);

			JS::JsRelease(myString, nullptr);
		}

		return undef;
	}

	JsValueRef CALLBACK loadModule(JsValueRef callee, bool isConstructor,
		JsValueRef* arguments, unsigned short argCount,
		void* callbackState) {
		auto thi = reinterpret_cast<JsScript*>(callbackState);

		JsValueRef undef;

		JS::JsGetUndefinedValue(&undef);

		if (!Chakra::VerifyArgCount(argCount, 2)) return undef;
		auto sz = Chakra::VerifyParameters({ {arguments[1], JsValueType::JsString} });
		if (!sz) {
			Chakra::ThrowError(sz.str);
			return undef;
		}

		auto wPath = Chakra::GetString(arguments[1]);

		for (auto& lib : thi->libraries) {
			if (lib->shouldInclude(wPath)) {
				JsValueRef global;
				JS::JsGetGlobalObject(&global);
				auto jsLib = lib->initialize(global);
				JS::JsRelease(global, nullptr);
				return jsLib;
			}
		}

		//if (wPath.starts_with(L"http") || wPath.starts_with(L"https")) {
		//	isNet = true;
		//}

		if (!wPath.ends_with(L".js")) {
			wPath += L".js";
		}

		auto path = thi->getFolderPath() / wPath;

		if (!std::filesystem::exists(path) || std::filesystem::is_directory(path)) {
			Chakra::ThrowError(L"Invalid filepath '" + path.wstring() + L"'");
			return undef;
		}

		auto mod = thi->getPlugin()->loadOrFindModule(thi, wPath);
		if (!mod) {
			Chakra::ThrowError(L"Unable to load module " + wPath);
			return Chakra::GetUndefined();
		}

		return mod->getModuleExports();
	}
}

void JsScript::loadScriptObjects() {
	JS::JsSetCurrentContext(ctx);
	int i = 0;
	this->objects.clear();
	this->objects.push_back(std::make_shared<ClientScriptingObject>(i++));
	this->objects.push_back(std::make_shared<GameScriptingObject>(i++));
	this->objects.push_back(std::make_shared<D2DScriptingObject>(i++));

	this->classes.clear();
	this->classes.push_back(std::make_shared<JsVec2>(this));
	this->classes.push_back(std::make_shared<JsVec3>(this));
	this->classes.push_back(std::make_shared<JsRect>(this));
	this->classes.push_back(std::make_shared<JsColor>(this));
	this->classes.push_back(std::make_shared<JsModuleClass>(this));
	this->classes.push_back(std::make_shared<JsHudModuleClass>(this));
	this->classes.push_back(std::make_shared<JsTextModuleClass>(this));
	this->classes.push_back(std::make_shared<JsSettingClass>(this));
	this->classes.push_back(std::make_shared<JsCommandClass>(this));
	this->classes.push_back(std::make_shared<JsEntityClass>(this));
	this->classes.push_back(std::make_shared<JsPlayerClass>(this));
	this->classes.push_back(std::make_shared<JsLocalPlayerClass>(this));
	this->classes.push_back(std::make_shared<JsItem>(this));
	this->classes.push_back(std::make_shared<JsItemStack>(this));
	this->classes.push_back(std::make_shared<JsTextureClass>(this));
#ifdef LATITE_DEBUG
	this->classes.push_back(std::make_shared<JsNativeModule>(this));
#endif
	JsErrorCode err;

	JsValueRef globalObj = Chakra::GetGlobalObject();

	// require();
	{
		Chakra::DefineFunc(globalObj, loadModule, L"require", this);
	}

	// sleep()
	{
		Chakra::DefineFunc(globalObj, sleepCallback, L"sleep");
	}

	// setTimeout()
	{
		Chakra::DefineFunc(globalObj, setTimeoutCallback, L"setTimeout", this);
	}

	// setInterval()
	{
		Chakra::DefineFunc(globalObj, setIntervalCallback, L"setInterval", this);
	}

	// Plugin
	{
		JsValueRef plugin;
		JS::JsCreateObject(&plugin);

		Chakra::SetPropertyString(plugin, L"name", data.name, true);
		Chakra::SetPropertyString(plugin, L"author", data.author, true);
		Chakra::SetPropertyString(plugin, L"description", data.description, true);
		Chakra::SetPropertyString(plugin, L"version", data.version, true);

		Chakra::SetProperty(globalObj, L"plugin", plugin, true);
	}

	{ // Log Func
		JsValueRef myPrint;
		err = JS::JsCreateFunction(clientMessageCallback, nullptr, &myPrint);

		JsPropertyIdRef propId;
		err = JS::JsGetPropertyIdFromName(L"clientMessage", &propId);
		err = JS::JsSetProperty(Chakra::GetGlobalObject(), propId, myPrint, false);

		err = JS::JsRelease(myPrint, nullptr);
	}

	for (auto& cl : classes) {
		Chakra::SetPropertyObject(globalObj, cl->getName(), cl->getConstructor());
		cl->createPrototype();
		cl->prepareFunctions();
	}

	for (auto& obj : objects) {
		obj->initialize(ctx, globalObj);
		JsPropertyIdRef propId;
		JS::JsGetPropertyIdFromName(obj->objName, &propId);
		JS::JsSetProperty(globalObj, propId, obj->object, false);
	}

	JS::JsRelease(globalObj, nullptr);
}

JsValueRef JsScript::AsyncOperation::call() {
	JS::JsSetCurrentContext(this->ctx);
	JsValueRef obj;
	this->args.insert(this->args.begin(), this->callback);
	Latite::getPluginManager().handleErrors(Chakra::CallFunction(this->callback, this->args.data(), static_cast<unsigned short>(this->args.size()), &obj));
	return obj;
}
