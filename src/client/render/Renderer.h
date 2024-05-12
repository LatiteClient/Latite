#pragma once
#include "util/DXUtil.h"
#include <vector>
#include <shared_mutex>

class Renderer final {
public:
	Renderer() = default;
	Renderer(Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	~Renderer();

	bool init(IDXGISwapChain* chain);
	bool hasInitialized() { return hasInit; };
	HRESULT reinit();
	void setShouldReinit();
	void setShouldInit();
	std::shared_lock<std::shared_mutex> lock();
	void render();
	bool isDX11ByDefault() { return isDX11; }

	int64_t mcePerf;
	int64_t arpPerf;
	int64_t d2dPerf;
	int64_t d3dPerf;
private:
	bool isDX11 = false;

	std::wstring fontFamily = L"Segoe UI";
	std::wstring fontFamily2 = L"Segoe UI";
	void releaseAllResources(bool indep = true);

	void createTextFormats();
	void releaseTextFormats();

	bool hasInit = false;
	bool shouldReinit = false;
	bool firstInit = false;
	bool shouldInit = false;
	bool reqCommandQueue = false;
	bool dx12Removed = false;
	bool hasCopiedBitmap = false;

	IDXGISwapChain* gameSwapChain;
	IDXGISwapChain4* swapChain4;

	ComPtr<IDXGIDevice> dxgiDevice;

	ComPtr<ID3D12Device> gameDevice12;
	ComPtr<ID3D11Device> gameDevice11;
	ID3D11Device* d3dDevice;
	ID3D12CommandQueue* commandQueue;

	ComPtr<ID3D11DeviceContext> d3dCtx;
	ComPtr<ID3D11On12Device> d3d11On12Device;

	ComPtr<ID2D1Factory3> d2dFactory;
	ComPtr<ID2D1Device> d2dDevice;
	ComPtr<ID2D1DeviceContext> d2dCtx;
	ComPtr<IDWriteFactory> dWriteFactory;
	ComPtr<IWICImagingFactory2> wicFactory;

	ComPtr<ID2D1SolidColorBrush> solidBrush;
	ComPtr<ID2D1Effect> shadowEffect;
	ComPtr<ID2D1Effect> affineTransformEffect;
	ComPtr<ID2D1Effect> blurEffect;

	ComPtr<IDWriteTextFormat> primaryFont;
	ComPtr<IDWriteTextFormat> primarySemilight;
	ComPtr<IDWriteTextFormat> primaryLight;
	ComPtr<IDWriteTextFormat> secondaryFont;
	ComPtr<IDWriteTextFormat> secondarySemilight;
	ComPtr<IDWriteTextFormat> secondaryLight;

	std::vector<ID3D12Resource*> d3d12Targets = {  };
	std::vector<ID3D11Resource*> d3d11Targets = {};
	std::vector<ID2D1Bitmap1*> renderTargets = {};

	std::vector<ID2D1Bitmap1*> blurBuffers = {};

	std::unordered_map<int64_t, std::pair<IDWriteTextFormat*, ComPtr<IDWriteTextLayout>>> cachedLayouts;

	std::shared_mutex mutex;
	int bufferCount = 3;
	float deltaTime = 1.f;
	std::chrono::system_clock::time_point lastTime;
public:
	ID2D1Bitmap1* testBitmap;

	enum class FontSelection {
		PrimaryRegular,
		PrimarySemilight,
		PrimaryLight,
		SecondaryRegular,
		SecondarySemilight,
		SecondaryLight,
	};

	void createDeviceIndependentResources();
	void createDeviceDependentResources();

	void releaseDeviceIndependentResources();
	void releaseDeviceResources();

	void updateSecondaryFont(std::wstring family) {
		fontFamily2 = std::move(family);

		auto mutex = lock();
		releaseTextFormats();
		createTextFormats();
	}

	void setDevice11(ID3D11Device* dev) {
		gameDevice11 = dev;
	}

	void setDevice12(ID3D12Device* dev) {
		gameDevice12 = dev;
	}

	void setCommandQueue(ID3D12CommandQueue* queue) noexcept {
		commandQueue = queue;
	}

	void setSwapChain(IDXGISwapChain* chain) noexcept {
		gameSwapChain = chain;
	}

	void clearTextCache() {
		this->cachedLayouts.clear();
	}

	[[nodiscard]] std::wstring getFontFamily() {
		return this->fontFamily;
	}

	void setFontFamily(std::wstring ws) {
		this->fontFamily = ws;
	}

	[[nodiscard]] std::wstring getFontFamily2() {
		return this->fontFamily2;
	}

	void setFontFamily2(std::wstring const& f2) {
		fontFamily2 = f2;
	}

	[[nodiscard]] D2D1_SIZE_F getScreenSize() {
		return { (float)d2dCtx->GetPixelSize().width, (float)d2dCtx->GetPixelSize().height };
	}

	[[nodiscard]] float getDeltaTime() {
		return deltaTime;
	}

	[[nodiscard]] IDWriteTextFormat* getTextFormat(FontSelection selection) {
		switch (selection) {
		case FontSelection::PrimaryRegular:
			return primaryFont.Get();
		case FontSelection::PrimarySemilight:
			return primarySemilight.Get();
		case FontSelection::PrimaryLight:
			return primaryLight.Get();
		case FontSelection::SecondaryRegular:
			return secondaryFont.Get();
		case FontSelection::SecondarySemilight:
			return secondarySemilight.Get();
		case FontSelection::SecondaryLight:
			return secondaryLight.Get();
		default:
			return nullptr;
		}
	}

	[[nodiscard]] ID2D1DeviceContext* getDeviceContext() {
		return d2dCtx.Get();
	}

	[[nodiscard]] ID2D1Device* getDevice() {
		return d2dDevice.Get();
	}

	[[nodiscard]] ID2D1SolidColorBrush* getSolidBrush() {
		return solidBrush.Get();
	}

	[[nodiscard]] ID2D1Factory3* getFactory() {
		return d2dFactory.Get();
	}

	[[nodiscard]] IDWriteFactory* getDWriteFactory() {
		return dWriteFactory.Get();
	}

	[[nodiscard]] ID2D1Bitmap1* getCopiedBitmap() {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		ID2D1Bitmap1* newBitmap = blurBuffers[0];

		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();
		if (!hasCopiedBitmap) {
			newBitmap->CopyFromBitmap(nullptr, myBitmap, nullptr);
			hasCopiedBitmap = true;
		}
		return newBitmap;
	}

	[[nodiscard]] ID2D1Bitmap1* copyCurrentBitmap() {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		ID2D1Bitmap1* newBitmap;

		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

		HRESULT hr = d2dCtx->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), &newBitmap);
		if (SUCCEEDED(hr)) {
			newBitmap->CopyFromBitmap(nullptr, myBitmap, nullptr);
		}
		return newBitmap;
	}

	[[nodiscard]] ID2D1Bitmap1* getCopiedBitmap(d2d::Rect const& rc) {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		ID2D1Bitmap1* newBitmap;

		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

		HRESULT hr = d2dCtx->CreateBitmap(bitmapSize, nullptr, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, pixelFormat), &newBitmap);
		if (SUCCEEDED(hr)) {
			auto pt = D2D1::Point2U((UINT32)rc.left, (UINT32)rc.top);
			auto urc = D2D1::RectU((UINT32)rc.left, (UINT32)rc.top, (UINT32)rc.right, (UINT32)rc.bottom);
			newBitmap->CopyFromBitmap(&pt, myBitmap, &urc);
		}
		return newBitmap;
	}

	[[nodiscard]] void getCopiedBitmap(ID2D1Bitmap1*& bmp, d2d::Rect const& rc) {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];

		D2D1_SIZE_U bitmapSize = myBitmap->GetPixelSize();
		D2D1_PIXEL_FORMAT pixelFormat = myBitmap->GetPixelFormat();

		auto pt = D2D1::Point2U((UINT32)rc.left, (UINT32)rc.top);
		auto urc = D2D1::RectU((UINT32)rc.left, (UINT32)rc.top, (UINT32)rc.right, (UINT32)rc.bottom);
		bmp->CopyFromBitmap(&pt, myBitmap, &urc);
	}

	[[nodiscard]] void getCopiedBitmap(ID2D1Bitmap1*& bmp) {
		auto idx = swapChain4->GetCurrentBackBufferIndex();
		ID2D1Bitmap1* myBitmap = this->renderTargets[idx];
		bmp->CopyFromBitmap(nullptr, myBitmap, nullptr);
	}

	[[nodiscard]] ID2D1Effect*& getShadowEffect() {
		return *shadowEffect.GetAddressOf();
	}

	[[nodiscard]] ID2D1Effect*& getAffineTransformEffect() {
		return *affineTransformEffect.GetAddressOf();
	}

	[[nodiscard]] ID2D1Effect*& getBlurEffect() {
		return *blurEffect.GetAddressOf();
	}

	[[nodiscard]] ID2D1Bitmap1* getBitmap() {
		auto buf = swapChain4->GetCurrentBackBufferIndex();
		return this->renderTargets[buf];
	}

	[[nodiscard]] ID2D1Bitmap1* getBlurBitmap() {
		return getCopiedBitmap();
	}

	[[nodiscard]] IWICImagingFactory2* getImagingFactory() {
		return this->wicFactory.Get();
	}

	[[nodiscard]] IDWriteTextLayout* getLayout(IDWriteTextFormat* fmt, std::wstring const& str, bool cache = false);
};
