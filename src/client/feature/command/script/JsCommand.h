#pragma once
#include "../Command.h"
#include "client/script/feature/JsEvented.h"

class JsCommand : public Command, public JsEvented {
public:
	JsCommand(std::string const& name, std::string const& description, std::string const& usage, std::vector<std::string> aliases = {})
		: Command(name, description, usage, aliases) {
		JS::JsGetCurrentContext(&ctx);
		this->eventListeners[L"execute"] = {};
		script = true;
	}

	virtual bool execute(std::string const label, std::vector<std::string> args) override;
	virtual bool tryRun(std::string const& label, std::vector<std::string> args, std::string const& line) override;

	JsContextRef ctx;
private:
};