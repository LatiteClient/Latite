#include "pch.h"
#include "Renderer.h"
#include "client/event/Eventing.h"
#include "client/event/events/RenderOverlayEvent.h"
#include "client/event/events/RendererCleanupEvent.h"
#include "client/event/events/RendererInitEvent.h"
#include "client/Latite.h"

namespace {
	const char* commandQueueTypeName(D3D12_COMMAND_LIST_TYPE type) noexcept {
		switch (type) {
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			return "DIRECT";
		case D3D12_COMMAND_LIST_TYPE_BUNDLE:
			return "BUNDLE";
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return "COMPUTE";
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return "COPY";
		default:
			return "UNKNOWN";
		}
	}

	bool sameComObject(IUnknown* left, IUnknown* right) {
		if (!left || !right) return left == right;

		ComPtr<IUnknown> leftUnknown;
		ComPtr<IUnknown> rightUnknown;
		if (FAILED(left->QueryInterface(IID_PPV_ARGS(&leftUnknown))) ||
			FAILED(right->QueryInterface(IID_PPV_ARGS(&rightUnknown)))) {
			return left == right;
		}

		return leftUnknown.Get() == rightUnknown.Get();
	}

	std::wstring selectedTextLocale() {
		try {
			auto const& languages = Latite::get().getL10nData().getLanguages();
			int selectedLanguage = Latite::get().getSelectedLanguage();
			if (selectedLanguage >= 0 && selectedLanguage < static_cast<int>(languages.size())) {
				return util::StrToWStr(languages[selectedLanguage]->langCode);
			}
		}
		catch (...) {
		}

		return L"en-us";
	}

	DWRITE_READING_DIRECTION selectedTextReadingDirection() {
		try {
			return Latite::get().getL10nData().isSelectedLanguageRightToLeft()
				? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT
				: DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
		}
		catch (...) {
			return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
		}
	}

	const char* dxgiErrorName(HRESULT hr) noexcept {
		switch (hr) {
		case DXGI_ERROR_SDK_COMPONENT_MISSING:
			return "DXGI_ERROR_SDK_COMPONENT_MISSING";
		default:
			return "unknown";
		}
	}
}

Renderer::~Renderer() {
	releaseAllResources(false, true, false);
}

void Renderer::setCommandQueue(ID3D12CommandQueue* queue) {
	if (!queue) {
		commandQueue = nullptr;
		return;
	}

	if (queue == commandQueue.Get()) return;
	if (queue == lastFailedCommandQueue.Get()) return;

	auto desc = queue->GetDesc();
	if (desc.Type != D3D12_COMMAND_LIST_TYPE_DIRECT) {
		return;
	}

	ComPtr<ID3D12Device> queueDevice;
	HRESULT deviceHr = queue->GetDevice(IID_PPV_ARGS(&queueDevice));
	if (FAILED(deviceHr) || !queueDevice) {
		return;
	}

	if (gameDevice12 && !sameComObject(gameDevice12.Get(), queueDevice.Get())) {
		return;
	}

	commandQueue = queue;
	lastFailedCommandQueue = nullptr;
}

bool Renderer::init(IDXGISwapChain* chain) {
	if (!shouldInit.load(std::memory_order_acquire)) return false;

	if (!chain) {
		Logger::Warn("Renderer init called with a null swap chain");
		return false;
	}

    this->gameSwapChain = chain;

	if (!swapChain4) {
		HRESULT swapChainHr = chain->QueryInterface(&swapChain4);
		if (FAILED(swapChainHr) || !swapChain4) {
			Logger::Fatal("IDXGISwapChain4 QueryInterface failed: hr=0x{:08X}, chain=0x{:X}",
				static_cast<unsigned>(swapChainHr),
				reinterpret_cast<uintptr_t>(chain));
			return false;
		}
	}

	const bool forceDX11 = Latite::get().shouldForceDX11();
	ComPtr<ID3D12Device> detectedDevice12;
	HRESULT device12Hr = chain->GetDevice(IID_PPV_ARGS(&detectedDevice12));
	if (SUCCEEDED(device12Hr) && detectedDevice12) {
		gameDevice12 = detectedDevice12;
		gameDevice11 = nullptr;
	}
	else {
		gameDevice12 = nullptr;
	}
	
	if (gameDevice12 && forceDX11 && !dx12Removed) {
		ComPtr<ID3D12Device5> device5;
		if (SUCCEEDED(gameDevice12.As(&device5))) {
			device5->RemoveDevice();
		}
		else {
			Logger::Warn("Force DX11 active, but ID3D12Device5 QueryInterface failed for device 0x{:X}",
				reinterpret_cast<uintptr_t>(gameDevice12.Get()));
		}

		bufferCount = 1;
		Logger::Info("Force DX11 active");
		dx12Removed = true;
		return false;
	}

	if (forceDX11 || !gameDevice12) {
		ComPtr<ID3D11Device> detectedDevice11;
		HRESULT device11Hr = chain->GetDevice(IID_PPV_ARGS(&detectedDevice11));
		if (FAILED(device11Hr) || !detectedDevice11) {
			Logger::Fatal("Failed to get D3D device from swap chain: D3D11 hr=0x{:08X}, D3D12 hr=0x{:08X}, forceDX11={}",
				static_cast<unsigned>(device11Hr),
				static_cast<unsigned>(device12Hr),
				forceDX11);
			return false;
		}

		Logger::Info("Using DX11");
		gameDevice11 = detectedDevice11;
		gameDevice12 = nullptr;
		commandQueue = nullptr;
		d3dDevice = gameDevice11;
		d3dDevice->GetImmediateContext(d3dCtx.ReleaseAndGetAddressOf());
		bufferCount = 1;
		isDX11 = true;
	}
	else {
		if (!commandQueue) {
			reqCommandQueue = true;
			return false;
		}

		auto queueDesc = commandQueue->GetDesc();
		ComPtr<ID3D12Device> queueDevice;
		HRESULT queueDeviceHr = commandQueue->GetDevice(IID_PPV_ARGS(&queueDevice));
		if (FAILED(queueDeviceHr) || !queueDevice) {
			Logger::Warn("Stored D3D12 command queue 0x{:X} GetDevice failed before D3D11On12 init: hr=0x{:08X}",
				reinterpret_cast<uintptr_t>(commandQueue.Get()),
				static_cast<unsigned>(queueDeviceHr));
			commandQueue = nullptr;
			reqCommandQueue = true;
			return false;
		}

		if (queueDesc.Type != D3D12_COMMAND_LIST_TYPE_DIRECT ||
			!sameComObject(gameDevice12.Get(), queueDevice.Get())) {
			Logger::Warn("Stored D3D12 command queue is not usable for D3D11On12: queue=0x{:X}, queueDevice=0x{:X}, swapChainDevice=0x{:X}, type={} ({})",
				reinterpret_cast<uintptr_t>(commandQueue.Get()),
				reinterpret_cast<uintptr_t>(queueDevice.Get()),
				reinterpret_cast<uintptr_t>(gameDevice12.Get()),
				commandQueueTypeName(queueDesc.Type),
				static_cast<int>(queueDesc.Type));
			commandQueue = nullptr;
			reqCommandQueue = true;
			return false;
		}

		reqCommandQueue = false;
		bufferCount = 3;
		isDX11 = false;
		Logger::Info("Using DX12");

#if LATITE_DEBUG
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
#endif

		IUnknown* queues[] = { commandQueue.Get() };
		d3dDevice = nullptr;
		d3dCtx = nullptr;
		d3d11On12Device = nullptr;

		auto createD3D11On12Device = [&](UINT flags) {
			d3dDevice = nullptr;
			d3dCtx = nullptr;
			d3d11On12Device = nullptr;

			return D3D11On12CreateDevice(gameDevice12.Get(),
				flags,
				nullptr, 0,
				queues,
				_countof(queues), 0, d3dDevice.ReleaseAndGetAddressOf(), d3dCtx.ReleaseAndGetAddressOf(),
				nullptr
			);
		};

		UINT on12Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef LATITE_DEBUG
		on12Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT on12Hr = createD3D11On12Device(on12Flags);

#ifdef LATITE_DEBUG
		if (on12Hr == DXGI_ERROR_SDK_COMPONENT_MISSING) {
			on12Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
			on12Hr = createD3D11On12Device(on12Flags);
		}
#endif

		if (FAILED(on12Hr) || !d3dDevice) {
			Logger::Fatal("D3D11On12CreateDevice failed: hr=0x{:08X} ({}), createFlags=0x{:X}, d3dDevice=0x{:X}, d3dCtx=0x{:X}, gameDevice12=0x{:X}, commandQueue=0x{:X}, queueDevice=0x{:X}, queueType={}, queueFlags=0x{:X}, queueNodeMask={}",
				static_cast<unsigned>(on12Hr),
				dxgiErrorName(on12Hr),
				on12Flags,
				reinterpret_cast<uintptr_t>(d3dDevice.Get()),
				reinterpret_cast<uintptr_t>(d3dCtx.Get()),
				reinterpret_cast<uintptr_t>(gameDevice12.Get()),
				reinterpret_cast<uintptr_t>(commandQueue.Get()),
				reinterpret_cast<uintptr_t>(queueDevice.Get()),
				commandQueueTypeName(queueDesc.Type),
				static_cast<unsigned>(queueDesc.Flags),
				queueDesc.NodeMask);
			lastFailedCommandQueue = commandQueue;
			commandQueue = nullptr;
			reqCommandQueue = true;
			return false;
		}

		HRESULT on12InterfaceHr = d3dDevice->QueryInterface(d3d11On12Device.ReleaseAndGetAddressOf());
		if (FAILED(on12InterfaceHr) || !d3d11On12Device) {
			Logger::Fatal("ID3D11On12Device QueryInterface failed: hr=0x{:08X}, d3dDevice=0x{:X}, commandQueue=0x{:X}",
				static_cast<unsigned>(on12InterfaceHr),
				reinterpret_cast<uintptr_t>(d3dDevice.Get()),
				reinterpret_cast<uintptr_t>(commandQueue.Get()));
			lastFailedCommandQueue = commandQueue;
			commandQueue = nullptr;
			reqCommandQueue = true;
			return false;
		}
	}

	if (!d3dDevice) {
		Logger::Fatal("Renderer init failed: no D3D11 device was created");
		return false;
	}

	createDeviceIndependentResources();

	HRESULT dxgiHr = d3dDevice->QueryInterface(dxgiDevice.ReleaseAndGetAddressOf());
	if (FAILED(dxgiHr) || !dxgiDevice) {
		Logger::Fatal("IDXGIDevice QueryInterface failed: hr=0x{:08X}, d3dDevice=0x{:X}",
			static_cast<unsigned>(dxgiHr),
			reinterpret_cast<uintptr_t>(d3dDevice.Get()));
		return false;
	}

	ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf()));
	ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, d2dCtx.GetAddressOf()));
	d2dCtx->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

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

	hasInit.store(true, std::memory_order_release);
	firstInit = true;

	RendererInitEvent ev{};
	Eventing::get().dispatch(ev);

    return true;
}

HRESULT Renderer::reinit() {
	releaseAllResources(true, false);
    hasInit.store(false, std::memory_order_release);
    return S_OK;
}

void Renderer::shutdownForEject() {
	releaseAllResources(true, true);
	hasInit.store(false, std::memory_order_release);
	shouldInit.store(false, std::memory_order_release);
	shouldReinit.store(false, std::memory_order_release);
}

void Renderer::setShouldReinit() {
	shouldReinit.store(true, std::memory_order_release);
}

void Renderer::setShouldInit() {
	shouldInit.store(true, std::memory_order_release);
}

void Renderer::beginResize() {
	if (activeResizes.fetch_add(1, std::memory_order_acq_rel) == 0) {
		auto rendererLock = lock();
		reinit();
	}
}

void Renderer::endResize() noexcept {
	activeResizes.fetch_sub(1, std::memory_order_acq_rel);
}

std::unique_lock<std::recursive_mutex> Renderer::lock() {
	return std::unique_lock<std::recursive_mutex>(mutex);
}

void Renderer::render() {
	if (gameDevice12) {
		ThrowIfFailed(gameDevice12->GetDeviceRemovedReason());
	}
	else {
		ThrowIfFailed(gameDevice11->GetDeviceRemovedReason());
	}

	if (shouldReinit.exchange(false, std::memory_order_acq_rel)) {
		reinit();
		return;
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);
	deltaTime = std::clamp(static_cast<float>(diff.count()) / 17.f, 0.02f, 20.f); // based on 60-ish FPS

	lastTime = now;

	if (!hasInit.load(std::memory_order_acquire)) return;

	auto idx = swapChain4->GetCurrentBackBufferIndex();
	if (gameDevice12) {
		d3d11On12Device->AcquireWrappedResources(&d3d11Targets[idx], 1);
	}


	d2dCtx->SetTarget(renderTargets[idx]);
	d2dCtx->BeginDraw();

	RenderOverlayEvent ev{ d2dCtx.Get() };
	Eventing::get().dispatch(ev);

	ThrowIfFailed(d2dCtx->EndDraw());

	if (gameDevice12) {
		d3d11On12Device->ReleaseWrappedResources(&d3d11Targets[idx], 1);
	}

	d3dCtx->Flush();
	this->hasCopiedBitmap = false;
}

void Renderer::releaseAllResources(bool flush, bool indep, bool dispatchCleanup) {
	if (dispatchCleanup) {
		RendererCleanupEvent ev{};
		Eventing::get().dispatch(ev);
	}

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

	releaseDeviceResources();

	for (auto& mb : this->blurBuffers) {
		SafeRelease(&mb);
	}
	this->blurBuffers.clear();

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

	gameDevice11 = nullptr;
	if (isDX12) gameDevice12 = nullptr;
	SafeRelease(&swapChain4);
	d3dDevice = nullptr;
	d2dDevice = nullptr;
	d2dCtx = nullptr;

	for (auto& i : d3d11Targets) {
		SafeRelease(&i);
	}
	d3d11Targets.clear();

	if (flush && d3dCtx) {
		d3dCtx->Flush();
	}

	dxgiDevice = nullptr;
	SafeRelease(&swapChain4);

	d3d11On12Device = nullptr;
	d3dCtx = nullptr;

	if (indep) releaseDeviceIndependentResources();
}

void Renderer::createTextFormats() {
	float fontSize = 10.f;
	std::wstring locale = selectedTextLocale();
	DWRITE_READING_DIRECTION readingDirection = selectedTextReadingDirection();
	auto configureTextFormat = [readingDirection](ComPtr<IDWriteTextFormat>& format) {
		if (!format.Get()) return;
		format->SetReadingDirection(readingDirection);
		format->SetFlowDirection(DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM);
	};

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		locale.c_str(),
		this->primaryFont.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_SEMI_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		locale.c_str(),
		this->primarySemilight.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		locale.c_str(),
		this->primaryLight.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily2.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		locale.c_str(),
		this->secondaryFont.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily2.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_SEMI_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		locale.c_str(),
		this->secondarySemilight.GetAddressOf()));

	ThrowIfFailed(dWriteFactory->CreateTextFormat(fontFamily2.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		locale.c_str(),
		this->secondaryLight.GetAddressOf()));

	configureTextFormat(primaryFont);
	configureTextFormat(primarySemilight);
	configureTextFormat(primaryLight);
	configureTextFormat(secondaryFont);
	configureTextFormat(secondarySemilight);
	configureTextFormat(secondaryLight);
}

void Renderer::releaseTextFormats() {
	primaryFont = nullptr;
	primaryLight = nullptr;
	primarySemilight = nullptr;
	secondaryFont = nullptr;
	secondaryLight = nullptr;
	secondarySemilight = nullptr;
}

void Renderer::createDeviceIndependentResources() {
	ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dWriteFactory.GetAddressOf())));
	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory3), (void**)d2dFactory.GetAddressOf()));
	
	createTextFormats();
	CoInitialize(nullptr);
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
	releaseTextFormats();
}

void Renderer::releaseDeviceResources() {
	solidBrush = nullptr;
	shadowEffect = nullptr;
	affineTransformEffect = nullptr;
	affineTransformEffect = nullptr;
	shadowEffect = nullptr;
	blurEffect = nullptr;
}

ComPtr<IDWriteTextLayout> Renderer::getLayout(IDWriteTextFormat* fmt, std::wstring const& str, bool cache) {
	auto hash = util::fnv1a_64w(str);
	if (cache) {
		auto it = this->cachedLayouts.find(hash);
		if (it != cachedLayouts.end()) {
			if (it->second.first == fmt) {
				return it->second.second;
			}
		}
	}

	auto [width, height] = getScreenSize();
	ComPtr<IDWriteTextLayout> layout;
	if (FAILED(dWriteFactory->CreateTextLayout(str.c_str(), static_cast<uint32_t>(str.size()), fmt, width, height, layout.GetAddressOf()))) {
		return {};
	}

	if (!cache) {
		return layout;
	}

	this->cachedLayouts[hash] = { fmt, layout };
	return layout;
}
