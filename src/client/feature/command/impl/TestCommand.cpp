#include "TestCommand.h"

TestCommand::TestCommand() : Command("test", "A command for testing", "[...]", {"tc"})
{
}

bool TestCommand::execute(std::string const label, std::vector<std::string> args)
{
	message("test");
	message("test", true);
	return true;
}
