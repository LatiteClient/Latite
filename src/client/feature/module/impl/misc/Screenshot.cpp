#include "pch.h"
#include "Screenshot.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "client/event/impl/UpdateEvent.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "util/DxContext.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"
#include "client/misc/ClientMessageSink.h"

#include <SHCore.h>

#include <winrt/windows.storage.h>
#include <winrt/windows.storage.streams.h>

using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;

Screenshot::Screenshot() : Module("Screenshot", "Screenshot", "Take a screenshot with a key.", GAME, nokeybind) {
    listen<KeyUpdateEvent>((EventListenerFunc)&Screenshot::onKey);
    listen<UpdateEvent>((EventListenerFunc)&Screenshot::onUpdate);
    listen<RenderOverlayEvent>((EventListenerFunc)&Screenshot::onRenderOverlay, false, 0 /*lowest priority so that Latite renders everything else*/);

    addSetting("ssKey", "Screenshot key", "The key you press to take a screenshot", this->screenshotKey);
}

namespace {
    bool shouldSleep = false;
}

void Screenshot::onKey(Event& evG) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evG);
	if (ev.isDown() && ev.getKey() == std::get<KeyValue>(this->screenshotKey)) {
		// take a screenshot
        queueToScreenshot = true;
        screenshotPath = util::GetLatitePath() / "Screenshots";
        std::filesystem::create_directory(screenshotPath);
        shouldSleep = true;
	}
}

void Screenshot::onRenderOverlay(Event& ev) {
    DXContext dc;
    if (queueToScreenshot) {
        takeScreenshot(screenshotPath);
        Latite::getClientMessageSink().push(std::format("Screenshot saved to {}", (screenshotPath / "screenshot.png").string()));
        queueToScreenshot = false;
    }

    if (savedBitmap) {
        auto now = std::chrono::system_clock::now();
        auto dist = now - startTime;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(dist).count() > 4000ll /*4 seconds*/) {
            this->savedBitmap = std::nullopt;
            this->lerpX = 0.f;
            this->lerpY = 0.f;
            this->flashLerp = 0.f;
            return;
        }
        if (flashLerp > 0.f) {
            // fade out
            flashLerp -= 0.05f * Latite::getRenderer().getDeltaTime();
            auto ss = Latite::getRenderer().getScreenSize();
            dc.fillRectangle({ 0.f, 0.f, ss.width, ss.height }, { 1.f, 1.f, 1.f, flashLerp });
        }
    }
}

void Screenshot::onUpdate(Event& ev) {
    if (shouldSleep) {
        //Sleep(200);
        shouldSleep = false;
    }
}

winrt::Windows::Foundation::IAsyncAction Screenshot::takeScreenshot(std::filesystem::path const& path) {
    // file
    auto folder = co_await StorageFolder::GetFolderFromPathAsync(path.wstring());
    auto file = co_await folder.CreateFileAsync(L"screenshot.png", CreationCollisionOption::OpenIfExists);
    ComPtr<ID2D1Bitmap1> bmp = Latite::getRenderer().copyCurrentBitmap();

    auto ctx = Latite::getRenderer().getDeviceContext();
    ctx->Flush();// MC won't let me flush (???)s

    IRandomAccessStream raStream = file.OpenAsync(FileAccessMode::ReadWrite).get();
    ComPtr<IStream> stream;
    ThrowIfFailed(
        CreateStreamOverRandomAccessStream(raStream.as<IUnknown>().get(), IID_PPV_ARGS(&stream))
    );

    auto wicFactory = Latite::getRenderer().getImagingFactory();

    // https://learn.microsoft.com/en-us/windows/win32/direct2d/save-direct2d-content-to-an-image-file

    ComPtr<IWICBitmapEncoder> wicBitmapEncoder;
    ThrowIfFailed(
        wicFactory->CreateEncoder(
            GUID_ContainerFormatPng,
            nullptr,    // No preferred codec vendor.
            wicBitmapEncoder.GetAddressOf()
        )
    );

    ThrowIfFailed(
        wicBitmapEncoder->Initialize(
            stream.Get(),
            WICBitmapEncoderNoCache
        )
    );

    ComPtr<IWICBitmapFrameEncode> wicFrameEncode;
    ThrowIfFailed(
        wicBitmapEncoder->CreateNewFrame(
            &wicFrameEncode,
            nullptr     // No encoder options.
        )
    );

    ThrowIfFailed(
        wicFrameEncode->Initialize(nullptr)
    );

    ComPtr<IWICImageEncoder> imageEncoder;
    ThrowIfFailed(
        wicFactory->CreateImageEncoder(
            Latite::getRenderer().getDevice(),
            &imageEncoder
        )
    );

    ThrowIfFailed(
        imageEncoder->WriteFrame(
            bmp.Get(),
            wicFrameEncode.Get(),
            nullptr     // Use default WICImageParameter options.
        )
    );

    ThrowIfFailed(
        wicFrameEncode->Commit()
    );

    ThrowIfFailed(
        wicBitmapEncoder->Commit()
    );

    // Flush all memory buffers to the next-level storage object.
    ThrowIfFailed(
        stream->Commit(STGC_DEFAULT)
    );

    savedBitmap = std::move(bmp);
    co_return;
}
