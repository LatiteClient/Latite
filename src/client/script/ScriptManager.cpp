// brace yourselves
#include "ScriptManager.h"
#include "client/Latite.h"
#include "client/misc/ClientMessageSink.h"
#include "util/Logger.h"
#include <memory>

std::shared_ptr<JsScript> ScriptManager::loadScript(std::wstring const& folderPath, bool run)
{
	auto fPathW = folderPath;
	auto scriptsPathW = util::GetLatitePath() / ("Scripts");
	auto scriptPath = scriptsPathW / fPathW / "index.js";
	if (!std::filesystem::exists(scriptPath)) return nullptr;

	for (auto& scr : this->items) {
		if (scr->relFolderPath == fPathW) {
			Latite::getClientMessageSink().push(util::Format(std::format("Script {} is already loaded.", util::WStrToStr(scr->data.name))));
			return nullptr;
		}
	}

	auto myScript = std::make_shared<JsScript>(scriptPath);
	myScript->relFolderPath = fPathW;
	if (!myScript->load()) return nullptr;
	myScript->loadScriptObjects();
	//loadScriptingObjects(myScript);
	this->items.push_back(myScript);

	if (run) {
		auto err = myScript->runScript();
		if (err != JsNoError) {
			if (err == JsErrorScriptException) {
				JsValueRef except;
				JS::JsGetAndClearException(&except);
				reportError(except, myScript->indexPath);
			}
			popScript(myScript);
			return nullptr;
		}
		myScript->fetchScriptData();
		Event::Value val{L"scriptName"};
		val.val= myScript->data.name;
		Event::Value val2{L"scriptVersion"};
		val2.val = myScript->data.version;
		Event::Value val3{L"scriptAuthor"};
		val3.val = myScript->data.author;
		Event newEv{ L"load-script", {val, val2, val3}, false };
		dispatchEvent(newEv);
	}
	return myScript;
}

std::shared_ptr<JsScript> ScriptManager::getScriptByName(std::wstring const& name)
{
	for (auto& script : items) {
		if (script->data.name == name) {
			return script;
		}
	}
	return nullptr;
}

void ScriptManager::popScript(std::shared_ptr<JsScript> ptr)
{
	for (auto it = items.begin(); it != items.end(); it++) {
		if (*it == ptr) {
			unloadScript(*it);
			items.erase(it);
			return;
		}
	}
}

void ScriptManager::reportError(JsValueRef except, std::wstring filePath) {

	auto str = util::WStrToStr(Chakra::ToString(except));
	//JsPropertyIdRef propId;
	//JS::JsGetPropertyIdFromName(L"stackTrace", &propId);
	//JsValueRef stackTrace;
	//JS::JsGetProperty(except, propId, &stackTrace);
	//auto line = Chakra::GetNumberProperty(stackTrace, L"line");

	std::stringstream ss;
	ss << "&cA runtime error occured in script " + util::WStrToStr(filePath) << ": " << str;

	Latite::getClientMessageSink().display(util::Format(ss.str()));
	//ClientMessageF(TextFormat::Format(TextFormat::RED) + "An error occured: " + util::WStrToStr(filePath) << ":" << static_cast<int>(line) << " " << str);
	// not sure if you release the exception or not, will do it anyway
	Chakra::Release(except);
}

void ScriptManager::handleErrors(JsErrorCode code) {
	JsContextRef ctx;
	JS::JsGetCurrentContext(&ctx);
	JsScript* script = nullptr;
	JS::JsGetContextData(ctx, (void**) & script);
	if (script && code == JsErrorScriptException) {
		JsValueRef except;
		JS::JsGetAndClearException(&except);
		reportError(except, script->indexPath);
	}
}

bool ScriptManager::loadPrerunScripts()
{
	if (!scriptingSupported()) {
		Logger::Warn("Scripting is not supported");
		return false;
	}

	auto prerunPath = util::GetLatitePath() / ("Scripts") / "Startup";
	std::filesystem::create_directory(prerunPath);

	using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
	for (auto& dirEntry : recursive_directory_iterator(prerunPath)) {
		if (dirEntry.is_directory()) {
			if (std::filesystem::exists(dirEntry.path().string() + "\\index.js")) {
				loadScript(std::wstring(L"Startup") + L"\\" + dirEntry.path().filename().wstring(), true);
			}
		}
	}
	return true;
}

void ScriptManager::runScriptingOperations()
{
	if (!scriptingSupported()) return;

	for (auto& scr : this->items) {
		scr->handleAsyncOperations();

		// timeouts
		for (auto it = scr->timeouts.begin(); it != scr->timeouts.end();) {
			auto now = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->createTime);
			if (duration.count() > it->time) {
				JsValueRef res;
				handleErrors(JS::JsCallFunction(it->callback, &it->callback, 1, &res));
				JS::JsRelease(res, nullptr);
				scr->timeouts.erase(it);
				continue;
			}
			++it;
		}

		// intervals
		for (auto& tim : scr->intervals) {
			auto now = std::chrono::system_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - tim.createTime);
			if (duration.count() > tim.time) {
				JsValueRef res;
				handleErrors(JS::JsCallFunction(tim.callback, &tim.callback, 1, &res));
				JS::JsRelease(res, nullptr);
				tim.createTime = now;
			}
		}
	}
}

void ScriptManager::init()
{
	auto scriptsPath = util::GetLatitePath() / ("Scripts");
	std::filesystem::create_directory(scriptsPath);

	initListeners();
	int id = 0;
	//this->objects.push_back(std::make_shared<ClientScriptingObject>(id++));
	//this->objects.push_back(std::make_shared<GameScriptingObject>(id++));
	//this->objects.push_back(std::make_shared<GraphicsScriptingObject>(id++));
	//this->objects.push_back(std::make_shared<Graphics3DScriptingObject>(id++));
}

void ScriptManager::initListeners()
{
	eventListeners[L"world-tick"] = {};
	eventListeners[L"leave-game"] = {};
	eventListeners[L"receive-chat"] = {};
	eventListeners[L"send-chat"] = {};
	eventListeners[L"render2d"] = {};
	eventListeners[L"attack"] = {};
	eventListeners[L"key-press"] = {};
	eventListeners[L"click"] = {};
	eventListeners[L"render3d"] = {};
	eventListeners[L"app-suspended"] = {};
	eventListeners[L"load-script"] = {};
	eventListeners[L"unload-script"] = {};
}

void ScriptManager::unloadScript(std::shared_ptr<JsScript> ptr)
{
	Event::Value val{L"scriptName"};
	val.name = L"scriptName";
	val.val = ptr->data.name;
	Event newEv{ L"unload-script", {val}, false };
	dispatchEvent(newEv);

	for (auto& ev : this->eventListeners) {
		if (ev.second.size() > 0)
			for (auto it = ev.second.begin(); it != ev.second.end();) {
				if (it->second == ptr->ctx) {
					JS::JsSetCurrentContext(it->second);
					unsigned int refCount;
					JS::JsRelease(it->first, &refCount);
					ev.second.erase(it);
				}
				else {
					++it;
				}
			}
	}
}

void ScriptManager::unloadAll() {
	for (auto& s : this->items) {
		popScript(s);
	}
}

bool ScriptManager::scriptingSupported() {
	return JS::JsAddRef;
}

void ScriptManager::uninitialize()
{
	for (auto& script : this->items) {
		popScript(script);
	}
}

bool ScriptManager::dispatchEvent(Event& ev)
{
	for (auto& lis : eventListeners) {
		if (lis.first == ev.type) {
			for (auto& l : lis.second) {
				JS::JsSetCurrentContext(l.second);
				JsValueRef params[2] = {};
				// create the obj
				JS::JsGetUndefinedValue(params);
				JS::JsCreateObject(&params[1]);
				JS::JsAddRef(params[1], nullptr);

				if (ev.isCancellable) {

					Event::Value val{};
					val.val = false;
					val.name = L"cancel";
					ev.values.push_back(val);
				}

				for (auto& val : ev.values) {
					JsPropertyIdRef idRef;

					JsValueRef ref = JS_INVALID_REFERENCE;
					switch (val.val.index()) {
					case Event::Value::Bool:
					{
						JS::JsBoolToBoolean(std::get<bool>(val.val), &ref);
					}
					break;
					case Event::Value::Number:
					{
						JS::JsDoubleToNumber(std::get<double>(val.val), &ref);
					}
					break;
					case Event::Value::String:
					{
						JS::JsPointerToString(std::get<std::wstring>(val.val).c_str(), std::get<std::wstring>(val.val).size(), &ref);
					}
					break;
					case Event::Value::EntityRef:

					{
						// TODO: Entity refs
					}
					break;
					default:
						throw std::runtime_error("unknown value");
						break;
					}
					JS::JsGetPropertyIdFromName(val.name.c_str(), &idRef);
					JS::JsSetProperty(params[1], idRef, ref, true);
					Chakra::Release(ref);
				}
				JsValueRef ret;

				unsigned int count;

				handleErrors(JS::JsCallFunction(l.first, params, 2, &ret));

				if (ev.isCancellable) {
					auto b = Chakra::GetBoolProperty(params[1], L"cancel");
					if (b) return true;
				}

				Chakra::Release(ret);
				Chakra::Release(params[1]);
				Chakra::Release(params[0]);
			}
			break;
		}
	}
	return false;
}
