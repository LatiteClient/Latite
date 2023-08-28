#include "TestCommand.h"
#include "pch.h"


TestCommand::TestCommand() : Command("test", "A command for testing", "{0} [...]", {"tc"}) {
}

bool TestCommand::execute(std::string const label, std::vector<std::string> args) {

#if 0
	winrt::hstring Title = L"title";
	winrt::hstring Content = L"djfiadhswfahsd9fasdf";

	winrt::Windows::UI::Popups::UICommand yesCommand;
	yesCommand.Label(L"custombutton1");
	auto yesId = yesCommand.Id();

	// Create No button
	winrt::Windows::UI::Popups::UICommand noCommand;
	noCommand.Label(L"custombutton2");
	auto noId = noCommand.Id();

	winrt::Windows::UI::Popups::MessageDialog dialog(Content, Title);
	dialog.Commands().Append(yesCommand);
	dialog.Commands().Append(noCommand);
#endif
	
	return true;
}
