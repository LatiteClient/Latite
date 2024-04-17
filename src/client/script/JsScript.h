#pragma once
#include <memory>
#include <functional>
#include <util/ChakraUtil.h>

class JsScript {
public:
	JsContextRef ctx;

	template <typename T>
	[[nodiscard]] T* getClass() {
		for (auto& cl : this->classes) {
			if (cl->getName() == T::class_name) return reinterpret_cast<T*>(cl.get());
		}
		//Logger::Fatal("Could not find scripting class {}!", T::class_name);
		return nullptr;
	}

	template <typename T>
	[[nodiscard]] T* getObject() {
		for (auto& cl : this->objects) {
			if (cl->id == T::objectID) return reinterpret_cast<T*>(cl.get());
		}
		//Logger::Fatal("Could not find scripting object {}!", T::class_name);
		return nullptr;
	}


	[[nodiscard]] class JsPlugin* getPlugin() { return plugin; };
	[[nodiscard]] std::filesystem::path getPath() { return path; }
	[[nodiscard]] std::filesystem::path getRelativePath() { return relPath; }
	[[nodiscard]] std::filesystem::path getFolderPath() { return path.parent_path(); }
	[[nodiscard]] JsContextRef getContext() { return ctx; }
	[[nodiscard]] JsValueRef getModuleExports();

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
		JsContextRef context;

		JsTimeout(int id, long long time, JsValueRef callback)
			: createTime(std::chrono::system_clock::now()),
			id(id), time(time), callback(callback) {
			JS::JsAddRef(callback, nullptr);
			JS::JsGetCurrentContext(&context);
		}

		~JsTimeout() {
			JS::JsRelease(callback, nullptr);
			JS::JsRelease(context, nullptr);
		}
	};

	

	JsScript(class JsPlugin* plugin, std::filesystem::path const& path, std::wstring const& relPath);

	bool load();
	JsErrorCode runScript();
	JsErrorCode compileScript();

	struct Resource {
		void* ptr;
		std::function<void(void* object)> finalize;

		Resource(void* ptr, decltype(finalize) finalizer) : ptr(ptr), finalize(finalizer) {}

		~Resource() {
			finalize(ptr);
		}
	};

	void addResource(void* object, decltype(Resource::finalize) finalizer) { this->resources.emplace_back(object, finalizer); };
	void removeResource(void* object) { 
		for (auto it = resources.begin(); it != resources.end(); ++it) {
			if (it->ptr == object) resources.erase(it);
		}
	}
protected:
	class JsPlugin* plugin;

	std::vector<std::shared_ptr<class ScriptingObject>> objects;
	std::vector<std::shared_ptr<class JsClass>> classes;

public:
	std::vector<std::shared_ptr<AsyncOperation>> pendingOperations;

	JsSourceContext sCtx = (JsSourceContext)0;

	std::vector<JsTimeout> timeouts = {};
	std::vector<JsTimeout> intervals = {};
	std::vector<std::shared_ptr<class JsLibrary>> libraries;

	void handleAsyncOperations();
private:

	std::filesystem::path path;
	std::wstring relPath;
	std::wstring relFolderPath;
	std::wstring loadedScript;
	std::wifstream stream;

	std::vector<JsValueRef> ownedEvents;
	std::vector<Resource> resources;

	void loadJSApi();
	void loadScriptObjects();

public:
	[[nodiscard]] static JsScript* getThis();
};