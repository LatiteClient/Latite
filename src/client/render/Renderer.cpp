#include "pch.h"
#include "Renderer.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "client/event/impl/RendererInitEvent.h"
#include "client/Latite.h"

Renderer::~Renderer() {
    // ...
	releaseAllResources();
}

bool Renderer::init(IDXGISwapChain* chain) {
	bool isDX12 = true;
	if (!dx12Removed && SUCCEEDED(chain->GetDevice(IID_PPV_ARGS(&gameDevice12))) && Latite::get().shouldForceDX11()) {
		static_cast<ID3D12Device5*>(gameDevice12.Get())->RemoveDevice();
		bufferCount = 1;
		Logger::Info("Force DX11 active");
		isDX12 = false;
		dx12Removed = true;
		return false;
	}
    this->gameSwapChain = chain;


	if (!swapChain4)
		ThrowIfFailed(chain->QueryInterface(&swapChain4));

	if (Latite::get().shouldForceDX11()) {
		isDX12 = false;
	}
	
	if (SUCCEEDED(chain->GetDevice(IID_PPV_ARGS(&gameDevice12))) && !dx12Removed && Latite::get().shouldForceDX11()) {
		static_cast<ID3D12Device5*>(gameDevice12.Get())->RemoveDevice();
		bufferCount = 1;
		Logger::Info("Force DX11 active");
		isDX12 = false;
		dx12Removed = true;
		return init(chain);
	}

	else gameDevice11 = nullptr;

	if (!gameDevice11.Get() || !gameDevice12.Get()) {
		if (Latite::get().shouldForceDX11() || FAILED(chain->GetDevice(IID_PPV_ARGS(&gameDevice12)))) {
			ThrowIfFailed(chain->GetDevice(IID_PPV_ARGS(&gameDevice11)));
			Logger::Info("Using DX11");
			d3dDevice = gameDevice11.Get();
			d3dDevice->GetImmediateContext(d3dCtx.GetAddressOf());
			bufferCount = 1;
			isDX11 = true;
			isDX12 = false;
		}
		else {
			Logger::Info("Using DX12");
			bufferCount = 3;
		}
	}

	if (gameDevice12.Get()) {
		if (!this->commandQueue) {
			return false;
		}
	}

	createDeviceIndependentResources();

#if LATITE_DEBUG
	if (gameDevice12) {
		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(gameDevice12->QueryInterface(IID_PPV_ARGS(&infoQueue))))
		{
			D3D12_MESSAGE_SEVERITY severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO,
			};

			D3D12_MESSAGE_ID denyIds[] =
			{
				D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
			};

			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;

			infoQueue->PushStorageFilter(&filter);
		}
    }
#endif
	if (gameDevice12) {
		D3D11On12CreateDevice(gameDevice12.Get(),
#ifdef LATITE_DEBUG
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG
#else
			D3D11_CREATE_DEVICE_BGRA_SUPPORT
#endif
			, nullptr, 0,
			reinterpret_cast<IUnknown**>(&commandQueue),
			1, 0, &d3dDevice, d3dCtx.GetAddressOf(),
			nullptr
		);
		d3dDevice->QueryInterface(d3d11On12Device.GetAddressOf());
		// TODO: is this necessary, we already reference the d3ddevice in 11on12createdevice
		d3dDevice->GetImmediateContext(&d3dCtx);
	}

	ThrowIfFailed(d3dDevice->QueryInterface(dxgiDevice.GetAddressOf()));
	ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf()));
	ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, d2dCtx.GetAddressOf()));

	renderTargets.clear();
	d3d11Targets.clear();
	for (int i = 0; i < bufferCount; i++) {
		ComPtr<IDXGISurface> surf;
		if (gameDevice12.Get()) {
			ID3D12Resource* backBuffer;
			ThrowIfFailed(swapChain4->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
			D3D11_RESOURCE_FLAGS flags{ D3D11_BIND_RENDER_TARGET };
			ID3D11Resource* res;
			ThrowIfFailed(d3d11On12Device->CreateWrappedResource(backBuffer, &flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, __uuidof(ID3D11Resource), (void**)&res));
			d3d11Targets.push_back(res);
			d3d12Targets.push_back(backBuffer);
		}
		else {
			ID3D11Resource* backBuffer;
			ThrowIfFailed(swapChain4->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
			d3d11Targets.push_back(backBuffer);
		}

		auto ss = d2dCtx->GetPixelSize();

		ThrowIfFailed(d3d11Targets[i]->QueryInterface(surf.GetAddressOf()));
		//auto info = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView(); // crashes
		float dpiX = 0;// info.LogicalDpi();
		float dpiY = 0;// info.LogicalDpi();
#pragma warning(push)
#pragma warning(disable : 4996)
		d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
#pragma warning(pop)

		D2D1_BITMAP_PROPERTIES1 prop = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX, dpiY);
		ID2D1Bitmap1* targ;
		ThrowIfFailed(d2dCtx->CreateBitmapFromDxgiSurface(surf.Get(), &prop, &targ));
		renderTargets.push_back(targ);
	}

	// brushes
	createDeviceDependentResources();

	// blur buffers
	blurBuffers = { 0 };

	auto idx = swapChain4->GetCurrentBackBufferIndex();
	ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
	ID2D1Bitmap1* bmp;
	D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
	D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

	ThrowIfFailed(d2dCtx->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), &bmp));
	this->blurBuffers[0] = bmp;
	this->hasCopiedBitmap = true;

	hasInit = true;
	firstInit = true;

	RendererInitEvent ev{};
	Eventing::get().dispatch(ev);

    return true;
}

HRESULT Renderer::reinit() {
	releaseAllResources();
    hasInit = false;
    return S_OK;
}

void Renderer::setShouldReinit() {
	shouldReinit = true;
}

std::shared_lock<std::shared_mutex> Renderer::lock() {
	return std::shared_lock<std::shared_mutex>(mutex);
}

void Renderer::render() {
	if (gameDevice12) {
		ThrowIfFailed(gameDevice12->GetDeviceRemovedReason());
	}
	else {
		ThrowIfFailed(gameDevice11->GetDeviceRemovedReason());
	}

	if (shouldReinit) {
		shouldReinit = false;
		reinit();
		return;
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
	deltaTime = std::clamp(static_cast<float>(diff.count()) / 17.f, 1.f, 20.f); // based on 60-ish FPS

	lastTime = now;

	if (!hasInit) return;

	auto idx = swapChain4->GetCurrentBackBufferIndex();
	if (gameDevice12) {
		std::chrono::high_resolution_clock::time_point perfCount = std::chrono::high_resolution_clock::now();
		d3d11On12Device->AcquireWrappedResources(&d3d11Targets[idx], 1);
		auto hnow = std::chrono::high_resolution_clock::now();
		this->arpPerf = std::chrono::duration_cast<std::chrono::microseconds>(hnow - perfCount).count();
	}


	std::chrono::high_resolution_clock::time_point perfCount = std::chrono::high_resolution_clock::now();

	d2dCtx->SetTarget(renderTargets[idx]);
	d2dCtx->BeginDraw();
	d2dCtx->SetTransform(D2D1::Matrix3x2F::Identity());
	d2dCtx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	RenderOverlayEvent ev{ d2dCtx.Get() };
	Eventing::get().dispatch(ev);

	d2dCtx->EndDraw();

	auto hnow = std::chrono::high_resolution_clock::now();

	this->d2dPerf = std::chrono::duration_cast<std::chrono::microseconds>(hnow - perfCount).count();

	perfCount = hnow;
	if (gameDevice12) {
		d3d11On12Device->ReleaseWrappedResources(&d3d11Targets[idx], 1);
	}

	d3dCtx->Flush();
	hnow = std::chrono::high_resolution_clock::now();

	this->d3dPerf = std::chrono::duration_cast<std::chrono::microseconds>(hnow - perfCount).count() + arpPerf;

	this->mcePerf = std::chrono::duration_cast<std::chrono::microseconds>(diff).count() - d3dPerf - d2dPerf;

	this->hasCopiedBitmap = false;
}

void Renderer::releaseAllResources() {
	RendererCleanupEvent ev{};
	Eventing::get().dispatch(ev);

	if (d2dCtx) d2dCtx->SetTarget(nullptr);

	bool isDX12 = gameDevice12;
	if (isDX12) {
		ID3D11RenderTargetView* nullViews[] = { nullptr, nullptr, nullptr };
		if (d3dCtx)
			d3dCtx->OMSetRenderTargets(3, nullViews, nullptr);
	}
	else {
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		if (d3dCtx)
			d3dCtx->OMSetRenderTargets(1, nullViews, nullptr);
	}

	cachedLayouts.clear();
	gameDevice11 = nullptr;

	for (auto& i : renderTargets) {
		SafeRelease(&i);
	}
	renderTargets.clear();

	if (isDX12) {
		for (auto& i : d3d12Targets) {
			SafeRelease(&i);
		}
		d3d12Targets.clear();
	}

	if (isDX12) gameDevice12 = nullptr;
	SafeRelease(&swapChain4);
	SafeRelease(&d3dDevice);
	d2dDevice = nullptr;
	d2dCtx = nullptr;

	for (auto& i : d3d11Targets) {
		SafeRelease(&i);
	}
	d3d11Targets.clear();

	releaseDeviceResources();

	if (d3dCtx) {
		d3dCtx->Flush();
	}

	dxgiDevice = nullptr;
	SafeRelease(&swapChain4);

	d3d11On12Device = nullptr;
	d3dCtx = nullptr;

	for (auto& mb : this->blurBuffers) {
		SafeRelease(&mb);
	}

	this->blurBuffers.clear();

	releaseDeviceIndependentResources();
}

void Renderer::createDeviceIndependentResources() {
	ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dWriteFactory.GetAddressOf())));
	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory3), (void**)d2dFactory.GetAddressOf()));
	float fontSize = 10.f;

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		this->segoe.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_SEMI_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		this->segoeSemilight.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		this->segoeLight.GetAddressOf()));

	//ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily2.c_str(),
	//	nullptr,
	//	DWRITE_FONT_WEIGHT_NORMAL,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	fontSize,
	//	L"en-us",
	//	this->font2.GetAddressOf()));
	//
	//ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily2.c_str(),
	//	nullptr,
	//	DWRITE_FONT_WEIGHT_SEMI_LIGHT,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	fontSize,
	//	L"en-us",
	//	this->font2Semilight.GetAddressOf()));
	//
	//ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily2.c_str(),
	//	nullptr,
	//	DWRITE_FONT_WEIGHT_LIGHT,
	//	DWRITE_FONT_STYLE_NORMAL,
	//	DWRITE_FONT_STRETCH_NORMAL,
	//	fontSize,
	//	L"en-us",
	//	this->font2Light.GetAddressOf()));

	//ThrowIfFailed(CoInitialize(nullptr));

	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory2), reinterpret_cast<void**>(this->wicFactory.GetAddressOf())));
}

void Renderer::createDeviceDependentResources() {
	D2D1_COLOR_F col = { 1.f, 0.f, 0.f, 1.f };
	ThrowIfFailed(d2dCtx->CreateSolidColorBrush(col, solidBrush.GetAddressOf()));
	ThrowIfFailed(d2dCtx->CreateEffect(CLSID_D2D1Shadow, this->shadowEffect.GetAddressOf()));
	ThrowIfFailed(d2dCtx->CreateEffect(CLSID_D2D12DAffineTransform, this->affineTransformEffect.GetAddressOf()));
	ThrowIfFailed(d2dCtx->CreateEffect(CLSID_D2D1GaussianBlur, this->blurEffect.GetAddressOf()));
}

void Renderer::releaseDeviceIndependentResources() {
	//CoUninitialize();
	dWriteFactory = nullptr;
	wicFactory = nullptr;
	d2dFactory = nullptr;
	segoe = nullptr;
	segoeLight = nullptr;
	segoeSemilight = nullptr;
	font2 = nullptr;
	font2Light = nullptr;
	font2Semilight = nullptr;
}

void Renderer::releaseDeviceResources() {
	solidBrush = nullptr;
	shadowEffect = nullptr;
	affineTransformEffect = nullptr;
	affineTransformEffect = nullptr;
	shadowEffect = nullptr;
	blurEffect = nullptr;
}

IDWriteTextLayout* Renderer::getLayout(IDWriteTextFormat* fmt, std::wstring const& str, bool cache) {
	auto hash = util::fnv1a_64w(str);
	auto it = this->cachedLayouts.find(hash);
	if (it != cachedLayouts.end()) {
		if (it->second.first == fmt) {
			return it->second.second.Get();
		}
	}

	auto [width, height] = getScreenSize();
	ComPtr<IDWriteTextLayout> layout;
	dWriteFactory->CreateTextLayout(str.c_str(), static_cast<uint32_t>(str.size()), fmt, width, height, layout.GetAddressOf());
	this->cachedLayouts[hash] = { fmt, layout };
	return layout.Get(); // Im pretty sure it implicitly adds a ref when I add it to cachedLayouts
}
