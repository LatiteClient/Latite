#pragma once
#include "api/manager/Manager.h"
#include "JsScript.h"
#include "ScriptingObject.h"

class ScriptManager final : public Manager<JsScript> {
private:
public:
	std::shared_ptr<JsScript> loadScript(std::string const& folderPath, bool run = true);
	std::shared_ptr<JsScript> getScriptByName(std::string const& name);

	void popScript(std::shared_ptr<JsScript> ptr);
	void reportError(JsValueRef except, std::wstring filePath);
	void handleErrors(JsErrorCode code);
	bool loadPrerunScripts();
	void runScriptingOperations();

	void init();
	void initListeners();
	void unloadScript(std::shared_ptr<JsScript> ptr);
	void unloadAll();

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
				Number, String, Bool, EntityRef
			};

			// TODO: switch to std::variant

			std::wstring name;
			ValueType type;
			double number_;
			std::wstring string_;
			int64_t entityref;

			bool bool_;
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