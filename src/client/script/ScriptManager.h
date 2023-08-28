#pragma once
#include "ScriptingObject.h"
#include "api/manager/Manager.h"
#include "api/eventing/Listenable.h"
#include <queue>
#include <variant>

class ScriptManager final : public Listener, public Manager<class JsScript> {
private:
	std::queue<std::shared_ptr<class JsScript>> scriptCheckQueue = {};
public:
	ScriptManager();

	std::shared_ptr<class JsScript> loadScript(std::wstring const& folderPath, bool run = true);
	std::shared_ptr<class JsScript> getScriptByName(std::wstring const& name);

	void popScript(std::shared_ptr<JsScript> ptr);
	void reportError(JsValueRef except, std::wstring filePath);
	void handleErrors(JsErrorCode code);
	bool loadPrerunScripts();
	void runScriptingOperations();
	std::optional<int> installScript(std::string const& name);

	void init();
	void initListeners();
	void unloadScript(std::shared_ptr<JsScript> ptr);
	void unloadAll();

	enum class Permission {
		SendChat,
		Operator,
	};

	bool hasPermission(JsScript* script, Permission perm);
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

	std::unordered_map<std::wstring, std::vector<std::pair<JsValueRef, JsContextRef>>> eventListeners;
	bool dispatchEvent(Event& ev);
	void uninitialize();
};