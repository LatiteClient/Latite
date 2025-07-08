#pragma once
#include "../Command.h"
#include "client/script/JsEvented.h"

class JsCommand : public Command, public JsEvented {
public:
	JsCommand(JsValueRef obj, std::string const& name, std::wstring const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: Command(name, description, usage, aliases),
		obj(obj) {
		JS::JsGetCurrentContext(&ctx);
		this->eventListeners[L"execute"] = {};
		JS::JsAddRef(obj, nullptr);
		script = true;
	}

	~JsCommand() {
		// FIXME: check if this actually doesnt cause a memory leak..
	}

	virtual bool execute(std::string const label, std::vector<std::string> args) override;
	virtual bool tryRun(std::string const& label, std::vector<std::string> args, std::string const& line) override;

	JsContextRef ctx;
	JsValueRef obj;
private:
};