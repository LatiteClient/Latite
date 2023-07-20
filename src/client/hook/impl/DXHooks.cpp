#include "DXHooks.h"
#include "util/DXUtil.h"
#include "client/Latite.h"

namespace {
	std::shared_ptr<Hook> PresentHook;
	std::shared_ptr<Hook> ResizeBuffersHook;
	std::shared_ptr<Hook> ExecuteCommandListsHook;
}

HRESULT __stdcall DXHooks::SwapChain_Present(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags) {
	return PresentHook->oFunc<decltype(&SwapChain_Present)>()(chain, SyncInterval, Flags);
}

HRESULT __stdcall DXHooks::SwapChain_ResizeBuffers(IDXGISwapChain* chain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	return ResizeBuffersHook->oFunc<decltype(&SwapChain_ResizeBuffers)>()(chain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

DXHooks::DXHooks() : HookGroup("DirectX") {
	ComPtr<IDXGIFactory1> factory;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> dctx;

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));
	ThrowIfFailed(factory->EnumAdapters(0, adapter.GetAddressOf()));

	D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };


	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = 100;
	swapChainDesc.BufferDesc.Height = 100;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	WNDCLASSEX wnd{ 0 };
	ZeroMemory(&wnd, sizeof(WNDCLASSEX));

	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.hInstance = Latite::get().dllInst;
	wnd.lpszClassName = L"dummywnd";
	wnd.lpfnWndProc = DefWindowProc;
	wnd.lpszMenuName = 0;
	wnd.style = CS_SAVEBITS | CS_DROPSHADOW;

	RegisterClassEx(&wnd);

	HWND hWnd = CreateWindowEx(0, L"dummywnd", L"hi", WS_MINIMIZEBOX,
		0, 0, 100, 100, nullptr, nullptr, Latite::get().dllInst, nullptr);

	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = TRUE;
	
	*lvl = D3D_FEATURE_LEVEL_11_1;

	D3D_FEATURE_LEVEL featureLevel;
	ThrowIfFailed(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, lvl, 1, D3D11_SDK_VERSION,
		&swapChainDesc, swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel, dctx.GetAddressOf()));

	uintptr_t* vftable = *reinterpret_cast<uintptr_t**>(swapChain.Get());

	DestroyWindow(hWnd);
	UnregisterClass(L"dummywnd", Latite::get().dllInst);

	PresentHook = addHook(vftable[8], SwapChain_Present, "IDXGISwapChain::Present");
	ResizeBuffersHook = addHook(vftable[13], SwapChain_ResizeBuffers, "IDXGISwapChain::ResizeBuffers");
}