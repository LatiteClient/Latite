#pragma once
#include "ScriptingObject.h"
#include "client/manager/Manager.h"
#include "client/event/Listener.h"
#include <queue>
#include <variant>

class PluginManager final : public Listener {
public:
	PluginManager();

	static std::filesystem::path getPluginsDir();
	static std::filesystem::path getPrerunPluginsDir();

	static std::expected<void, std::string> installScript(std::string const& name);

	std::shared_ptr<class JsPlugin> loadPlugin(std::wstring const& folderPath, bool run = true);
	std::shared_ptr<class JsPlugin> getPluginByName(std::wstring const& name);
	std::shared_ptr<class JsPlugin> getPluginById(std::string const& id);

	[[nodiscard]] static bool isPluginInstalled(std::string const& id);

	void forEach(std::function<void(std::shared_ptr<JsPlugin>)> callback) {
		for (auto& plugin : items | std::views::values) {
			callback(plugin);
		}
	}

	void popScript(std::shared_ptr<JsPlugin> ptr);
	void reportError(JsValueRef except, std::wstring filePath);
	void handleErrors(JsErrorCode code);
	bool loadPrerunScripts();
	void runScriptingOperations();

	struct PluginInfo {
		std::string id;
		std::wstring name, author, version, desc;
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


	bool dispatchEvent(Event& ev);
	void uninitialize();
	std::unordered_map<std::wstring, std::vector<std::tuple<int, JsValueRef, JsContextRef>>> eventListeners;
private:
	std::map<std::string, std::shared_ptr<JsPlugin>> items;

};