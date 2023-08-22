/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../../Module.h"
#include "client/event/Eventing.h"
#include <filesystem>
#include <Windows.h>

#include <winrt/Windows.Foundation.h>

class Screenshot : public Module {
public:
	Screenshot();
private:
	void onKey(Event& ev);
	winrt::Windows::Foundation::IAsyncAction takeScreenshot(std::filesystem::path const& path);
	KeyValue screenshotKey = KeyValue(VK_F2);
};