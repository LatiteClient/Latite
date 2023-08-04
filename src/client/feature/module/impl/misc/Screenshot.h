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