#include "Renderer.h"
#include "util/Logger.h"
#include "client/event/Eventing.h"
#include "client/event/impl/RenderOverlayEvent.h"
#include "client/event/impl/RendererCleanupEvent.h"
#include "client/event/impl/RendererInitEvent.h"

Renderer::~Renderer() {
    // ...
	releaseAllResources();
}

bool Renderer::init(IDXGISwapChain* chain) {
    this->gameSwapChain = chain;
	if (!swapChain4)
		ThrowIfFailed(chain->QueryInterface(&swapChain4));

	if (!gameDevice11.Get() || !gameDevice12.Get()) {
		if (FAILED(chain->GetDevice(IID_PPV_ARGS(&gameDevice12)))) {
			ThrowIfFailed(chain->GetDevice(IID_PPV_ARGS(&gameDevice11)));
			Logger::Info("Using DX11");
			d3dDevice = gameDevice11.Get();
			d3dDevice->GetImmediateContext(d3dCtx.GetAddressOf());
			bufferCount = 1;
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
#ifdef DEBUG
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
	ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2dCtx.GetAddressOf()));

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

		auto ss = d2dCtx->GetSize();

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
	for (int i = 0; i < this->bufferCount; ++i) {
		auto bmp = copyCurrentBitmap();
		this->blurBuffers.push_back(bmp);
	}

	hasInit = true;
	firstInit = true;

	RendererInitEvent ev{};
	Eventing::get().dispatchEvent(ev);

    return true;
}

HRESULT Renderer::reinit() {
	releaseAllResources();
    hasInit = false;
    return S_OK;
}

std::shared_lock<std::shared_mutex> Renderer::lock() {
	return std::shared_lock<std::shared_mutex>(mutex);
}

void Renderer::render() {
	auto idx = swapChain4->GetCurrentBackBufferIndex();
	if (gameDevice12) {
		d3d11On12Device->AcquireWrappedResources(&d3d11Targets[idx], 1);
	}

	auto bmp = blurBuffers[idx];
	// Update the current blur buffer
	bmp->CopyFromBitmap(nullptr, renderTargets[idx], nullptr);

	d2dCtx->SetTarget(renderTargets[idx]);
	d2dCtx->BeginDraw();
	d2dCtx->SetTransform(D2D1::Matrix3x2F::Identity());

	RenderOverlayEvent ev{ d2dCtx.Get() };
	Eventing::get().dispatchEvent(ev);

	d2dCtx->EndDraw();

	if (gameDevice12) {
		d3d11On12Device->ReleaseWrappedResources(&d3d11Targets[idx], 1);
	}

	d3dCtx->Flush();
}

void Renderer::releaseAllResources() {
	RendererCleanupEvent ev{};
	Eventing::get().dispatchEvent(ev);

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
		this->font.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_SEMI_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		this->fontSemilight.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"en-us",
		this->fontLight.GetAddressOf()));

	//ThrowIfFailed(CoInitialize(nullptr));

	ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), reinterpret_cast<void**>(this->wicFactory.GetAddressOf())));
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
	font = nullptr;
	fontLight = nullptr;
	fontSemilight = nullptr;
}

void Renderer::releaseDeviceResources() {
	solidBrush = nullptr;
	shadowEffect = nullptr;
	affineTransformEffect = nullptr;
	affineTransformEffect = nullptr;
	shadowEffect = nullptr;
	blurEffect = nullptr;
}
