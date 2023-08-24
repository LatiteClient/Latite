#pragma once
#include "../../Module.h"
#include "util/DxUtil.h"
#include "client/event/Eventing.h"
#include <filesystem>
#include <Windows.h>

#include <winrt/Windows.Foundation.h>

class Screenshot : public Module {
public:
	Screenshot();
private:
	void onKey(Event& ev);
	void onRenderOverlay(Event& ev);
	winrt::Windows::Foundation::IAsyncAction takeScreenshot(std::filesystem::path const& path);
	ValueType screenshotKey = KeyValue(VK_F2);

	bool queueToScreenshot = false;
	std::filesystem::path screenshotPath{};

	std::optional<ComPtr<ID2D1Bitmap1>> savedBitmap = std::nullopt;
	float lerpX = 0.f;
	float lerpY = 0.f;
	float flashLerp = 1.f;
	std::chrono::system_clock::time_point startTime;
};