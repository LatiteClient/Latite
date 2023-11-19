#pragma once
#include "../Command.h"
#include "client/script/feature/JsEvented.h"

class JsCommand : public Command, public JsEvented {
public:
	JsCommand(JsValueRef obj, std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: Command(name, description, usage, aliases),
		obj(obj) {
		JS::JsGetCurrentContext(&ctx);
		this->eventListeners[L"execute"] = {};
		JS::JsAddRef(obj, nullptr);
		script = true;
	}

	~JsCommand() {
		JS::JsRelease(obj, nullptr);
		// yes, we release it twice (crazy stuff)
		JS::JsRelease(obj, nullptr);
	}

	virtual bool execute(std::string const label, std::vector<std::string> args) override;
	virtual bool tryRun(std::string const& label, std::vector<std::string> args, std::string const& line) override;

	JsContextRef ctx;
	JsValueRef obj;
private:
};