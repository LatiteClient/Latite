#pragma once
#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include "chakra/ChakraCore.h"
#include "lib/JsLibrary.h"
#include "ScriptingObject.h"
#include "class/JsClass.h"

class JsScript final {
	std::vector<std::shared_ptr<ScriptingObject>> objects;
	std::vector<std::shared_ptr<JsClass>> classes;
public:
	JsContextRef ctx;
	JsSourceContext sCtx = 1;
	JsRuntimeHandle runtime;
	std::string runError = "";

	std::wstring indexPath;
	std::wstring relFolderPath;
	std::wifstream indexStream;
	std::wstring loadedScript;

	std::vector<JsValueRef> ownedEvents;
	std::vector<std::shared_ptr<JsLibrary>> libraries;

	struct ScriptDefinedData {
		std::wstring name;
		std::wstring version;
		std::wstring author;
		std::wstring description;
	} data;

	class AsyncOperation {
	public:
		bool flagDone = false;
		bool hasCleared = false;
		bool shouldRemove = false;
		JsContextRef ctx = JS_INVALID_REFERENCE;
		JsValueRef callback = JS_INVALID_REFERENCE;
		std::vector<JsValueRef> args;
		void(*initFunc)(AsyncOperation*);
		//std::function<void(AsyncOperation& op)> checkFunc;
		std::chrono::system_clock::time_point createTime = {};
		void* param;
		std::vector<JsValueRef> params;
		std::shared_ptr<std::thread> thr = nullptr;

		JsValueRef call();
		virtual void getArgs() {};

		AsyncOperation(bool shouldRemove, JsValueRef callback, decltype(initFunc) initFunc, void* param = nullptr) : shouldRemove(shouldRemove), args({}), callback(callback), initFunc(initFunc),
			createTime(std::chrono::system_clock::now()), param(param), params({}) {
			JS::JsGetCurrentContext(&ctx);
			JS::JsAddRef(callback, nullptr);

			for (auto& arg : this->params) {
				JS::JsRelease(arg, nullptr);
			}
		}

		void run() {
			thr = std::make_shared<std::thread>(std::thread(initFunc, this));
			thr->detach();
		}

		~AsyncOperation() {
			JS::JsRelease(callback, nullptr);
		}
	};

	struct JsTimeout {
		std::chrono::system_clock::time_point createTime = {};
		long long time;
		int id;
		JsValueRef callback;

		JsTimeout(int id, long long time, JsValueRef callback)
			: createTime(std::chrono::system_clock::now()),
			id(id), time(time), callback(callback) {
			JS::JsAddRef(callback, nullptr);
		}

		~JsTimeout() {
			JS::JsRelease(callback, nullptr);
		}
	};

	std::vector<std::shared_ptr<AsyncOperation>> pendingOperations;
	std::vector<JsTimeout> timeouts = {};
	std::vector<JsTimeout> intervals = {};

	JsScript(std::wstring const& indexPath);
	bool load();
	bool shouldRemove();
	
	template <typename T>
	[[nodiscard]] T* findClass(std::wstring const& name) {
		for (auto& cl : this->classes) {
			if (cl->getName() == name) return reinterpret_cast<T*>(cl.get());
		}
		return nullptr;
	}

	void loadJSApi();
	void loadScriptObjects();
	void fetchScriptData();
	void unload();
	void handleAsyncOperations();

	JsErrorCode runScript();

	~JsScript() { if (runtime != JS_INVALID_RUNTIME_HANDLE) unload(); }
};