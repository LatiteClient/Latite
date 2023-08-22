/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#include "TestCommand.h"


#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/impl/windows.web.http.2.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/impl/windows.foundation.collections.1.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/windows.system.h>
#include <winrt/windows.ui.popups.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/windows.ui.viewmanagement.h>
#include <winrt/windows.ui.core.h>

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
