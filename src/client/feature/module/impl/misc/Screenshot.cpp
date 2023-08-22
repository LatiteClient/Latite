#include "Screenshot.h"
#include "client/event/impl/KeyUpdateEvent.h"
#include "util/Util.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"
#include "client/misc/ClientMessageSink.h"

#include <SHCore.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/impl/windows.web.http.2.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/windows.storage.h>
#include <winrt/windows.storage.streams.h>


using namespace winrt;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Web::Http::Filters;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Storage;

#define REF &

Screenshot::Screenshot() : Module("Screenshot", "Screenshot Key", "Take a screenshot with a key.", GAME, nokeybind) {
	listen<KeyUpdateEvent>((EventListenerFunc) REF Screenshot::onKey);
}

void Screenshot::onKey(Event& evG) {
	auto& ev = reinterpret_cast<KeyUpdateEvent&>(evG);
	if (ev.isDown() && ev.getKey() == this->screenshotKey) {
		// take a screenshot
        auto path = util::GetLatitePath() / "Screenshots";
        std::filesystem::create_directory(path);
		takeScreenshot(util::GetLatitePath());
        Latite::getClientMessageSink().push(std::format("Screenshot saved to {}", (path / "screenshot.png").string()));
	}
}

winrt::Windows::Foundation::IAsyncAction Screenshot::takeScreenshot(std::filesystem::path const& path) {
    // file
    auto folder = co_await StorageFolder::GetFolderFromPathAsync(path.wstring());
    auto file = co_await folder.CreateFileAsync(L"screenshot.png", CreationCollisionOption::OpenIfExists);
    auto bmp = Latite::getRenderer().getBitmap();

    IRandomAccessStream raStream = file.OpenAsync(FileAccessMode::ReadWrite).get();

    ComPtr<IStream> stream;
    ThrowIfFailed(
        CreateStreamOverRandomAccessStream((IUnknown*) & raStream, IID_PPV_ARGS(&stream))
    );

    auto wicFactory = Latite::getRenderer().getImagingFactory();
    auto ctx = Latite::getRenderer().getDeviceContext();
    ctx->Flush();

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
            bmp,
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

    co_return;
}
