#pragma once
#include "api/manager/Manager.h"
#include "api/eventing/Listenable.h"
#include "JsScript.h"
#include "ScriptingObject.h"
#include <queue>
#include <variant>

class ScriptManager final : public Listener, public Manager<JsScript> {
private:
	std::queue<std::shared_ptr<JsScript>> scriptCheckQueue = {};
public:
	std::shared_ptr<JsScript> loadScript(std::wstring const& folderPath, bool run = true);
	std::shared_ptr<JsScript> getScriptByName(std::wstring const& name);

	void popScript(std::shared_ptr<JsScript> ptr);
	void reportError(JsValueRef except, std::wstring filePath);
	void handleErrors(JsErrorCode code);
	bool loadPrerunScripts();
	void runScriptingOperations();

	void init();
	void initListeners();
	void unloadScript(std::shared_ptr<JsScript> ptr);
	void unloadAll();
	
	static bool scriptingSupported();

	using event_callback_t = void(__fastcall*)(JsValueRef func);

	enum class EventType {
		WorldTick,
		LeaveGame,
		SendChat,
		ReceiveChat
	};

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