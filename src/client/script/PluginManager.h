#pragma once
#include "ScriptingObject.h"
#include "client/manager/Manager.h"
#include "client/event/Listener.h"
#include <queue>
#include <variant>

class PluginManager final : public Listener, public Manager<class JsPlugin> {
private:
	std::queue<std::shared_ptr<class JsPlugin>> scriptCheckQueue = {};
public:
	PluginManager();

	static std::filesystem::path getUserDir();
	static std::filesystem::path getUserPrerunDir();

	std::shared_ptr<class JsPlugin> loadPlugin(std::wstring const& folderPath, bool run = true);
	std::shared_ptr<class JsPlugin> getPluginByName(std::wstring const& name);

	void popScript(std::shared_ptr<JsPlugin> ptr);
	void reportError(JsValueRef except, std::wstring filePath);
	void handleErrors(JsErrorCode code);
	bool loadPrerunScripts();
	void runScriptingOperations();
	std::optional<int> installScript(std::string const& name);

	struct PluginInfo {
		std::wstring id, name, author, version, desc;
	};
	std::vector<PluginInfo> fetchPluginsFromMarket();
	std::vector<PluginInfo> fetchPlugins();

	void init();
	void initListeners();
	void unloadScript(std::shared_ptr<JsPlugin> ptr);
	void unloadAll();

	enum class Permission {
		SendChat,
		Operator,
	};

	bool hasPermission(JsPlugin* script, Permission perm);
	static bool scriptingSupported();

	using event_callback_t = void(__fastcall*)(JsValueRef func);

	struct Event {
		std::wstring type;
		struct Value {
			enum ValueType {
				Number = 1, 
				String, 
				EntityRef, 
				Bool
			};

			std::wstring name;
			std::variant<std::nullptr_t, double, std::wstring, int64_t, bool> val = nullptr;

			Value(std::wstring const& name) : name(name) {
			}

			Value() {
			}

			~Value() {
			}
		};
		std::vector<Value> values = {};
		bool cancel;
		bool isCancellable;

		Event(const std::wstring& type, const std::vector<Value>& values, bool cancellable)
			: type(type), values(values), cancel(false), isCancellable(cancellable)
		{
		}
	};

	std::unordered_map<std::wstring, std::vector<std::tuple<int, JsValueRef, JsContextRef>>> eventListeners;
	bool dispatchEvent(Event& ev);
	void uninitialize();
};