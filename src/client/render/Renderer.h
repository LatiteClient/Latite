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
	bool hasInit() { return hasInit; };
	HRESULT reinit();
	std::shared_lock<std::shared_mutex> lock();
private:
	void releaseAllResources();

	bool hasInit = false;
	bool firstInit = false;
	bool reqCommandQueue = false;

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
	ComPtr<IWICImagingFactory> wicFactory;
	ComPtr<ID2D1SolidColorBrush> solidBrush;

	ComPtr<IDWriteTextFormat> segoeUI;
	ComPtr<IDWriteTextFormat> segoeUISemilight;
	ComPtr<IDWriteTextFormat> segoeUILight;

	std::vector<ID3D12Resource*> d3d12Targets = {  };
	std::vector<ID3D11Resource*> d3d11Targets = {};
	std::vector<ID2D1Bitmap1*> renderTargets = {};

	std::vector<ID2D1Bitmap1*> motionBlurBitmaps = {};

	std::shared_mutex mutex;
	int bufferCount = 3;
public:
	enum class FontSelection {
		Regular,
		Semilight,
		Light
	};

	HRESULT createDeviceIndependentResources();
	HRESULT createDeviceDependentResources();

	void releaseDeviceIndependentResources();
	void releaseDeviceResources();

	void setDevice11(ID3D11Device* dev) noexcept {
		gameDevice11 = dev;
	}

	void setGame11On12(ID3D11On12Device* dev) noexcept {
		d3d11On12Device = dev;
	}

	void setDevice12(ID3D12Device* dev) noexcept {
		gameDevice12 = dev;
	}

	void setCommandQueue(ID3D12CommandQueue* queue) noexcept {
		commandQueue = queue;
	}

	void setCommandQueue(ID3D12CommandQueue* queue) {
		commandQueue = queue;
	}

	void setSwapChain(IDXGISwapChain* chain) noexcept {
		gameSwapChain = chain;
	}


	[[nodiscard]] IDWriteTextFormat* getTextFormat(FontSelection selection) {
		switch (selection) {
		case FontSelection::Regular:
			return segoeUI.Get();
		case FontSelection::Semilight:
			return segoeUISemilight.Get();
		case FontSelection::Light:
			return segoeUILight.Get();
		default:
			return nullptr;
		}
	}

	[[nodiscard]] ID2D1DeviceContext* getDeviceContext() {
		return d2dCtx.Get();
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

};
