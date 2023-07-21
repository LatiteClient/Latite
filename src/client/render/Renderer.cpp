#include "Renderer.h"
#include "util/Logger.h"

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
			Logger::info("Using DX11");
			d3dDevice = gameDevice11.Get();
			d3dDevice->GetImmediateContext(d3dCtx.GetAddressOf());
			bufferCount = 1;
		}
		else {
			Logger::info("Using DX12");
			bufferCount = 3;
		}
	}

	if (gameDevice12.Get()) {
		if (!this->commandQueue) {
			return false;
		}
	}

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
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX, dpiY);
		ID2D1Bitmap1* targ;
		ThrowIfFailed(d2dCtx->CreateBitmapFromDxgiSurface(surf.Get(), &prop, &targ));
		renderTargets.push_back(targ);
	}

	// brushes
	createDeviceDependentResources();

	hasInit = true;
	firstInit = true;

    return true;
}

HRESULT Renderer::reinit() {
    hasInit = false;

    return E_NOTIMPL;
}

std::shared_lock<std::shared_mutex> Renderer::lock()
{
	return std::shared_lock<std::shared_mutex>(mutex);
}

void Renderer::releaseAllResources() {
	// NOTE: release Dxgidevice, swapchainv4 (I think?)
}
