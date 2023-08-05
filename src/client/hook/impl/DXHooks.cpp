#include "DXHooks.h"
#include "util/DXUtil.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"

namespace {
	std::shared_ptr<Hook> PresentHook;
	std::shared_ptr<Hook> ResizeBuffersHook;
	std::shared_ptr<Hook> ExecuteCommandListsHook;
}

HRESULT __stdcall DXHooks::SwapChain_Present(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags) {
	if (!Latite::getRenderer().hasInitialized()) {
		auto lock = Latite::getRenderer().lock();
		Latite::getRenderer().init(chain);
	}

	if (Latite::getRenderer().hasInitialized()) {
		auto lock = Latite::getRenderer().lock();
		Latite::getRenderer().render();
	}

	return PresentHook->oFunc<decltype(&SwapChain_Present)>()(chain, SyncInterval, Flags);
}

HRESULT __stdcall DXHooks::SwapChain_ResizeBuffers(IDXGISwapChain* chain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	Latite::getRenderer().reinit();
	return ResizeBuffersHook->oFunc<decltype(&SwapChain_ResizeBuffers)>()(chain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT __stdcall DXHooks::CommandQueue_ExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
	auto lock = Latite::getRenderer().lock();
	Latite::getRenderer().setCommandQueue(queue);
	return ExecuteCommandListsHook->oFunc<decltype(&CommandQueue_ExecuteCommandLists)>()(queue, NumCommandLists, ppCommandLists);
}

DXHooks::DXHooks() : HookGroup("DirectX") {
	ComPtr<IDXGIFactory1> factory;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<IDXGIAdapter> adapter;
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D12Device> device12;
	ComPtr<ID3D11DeviceContext> dctx;
	ComPtr<ID3D12CommandQueue> cqueue;

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));
	ThrowIfFailed(factory->EnumAdapters(0, adapter.GetAddressOf()));



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
	
	D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };

	D3D_FEATURE_LEVEL featureLevel;
	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, lvl, 1, D3D11_SDK_VERSION,
		&swapChainDesc, swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel, dctx.GetAddressOf()))) {

		lvl[1] = D3D_FEATURE_LEVEL_10_0;

		ThrowIfFailed(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, lvl, 1, D3D11_SDK_VERSION,
			&swapChainDesc, swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel, dctx.GetAddressOf()));

	}

	uintptr_t* vftable = *reinterpret_cast<uintptr_t**>(swapChain.Get());
	uintptr_t* cqueueVftable = nullptr;

	{

		//DX12 only
		// 
		// dummy device
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device12)))) {

			D3D12_COMMAND_QUEUE_DESC queueDesc = {};
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			ThrowIfFailed(device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cqueue)));
			cqueueVftable = *reinterpret_cast<uintptr_t**>(cqueue.Get());
		}
	}

	DestroyWindow(hWnd);
	UnregisterClass(L"dummywnd", Latite::get().dllInst);

	PresentHook = addHook(vftable[8], SwapChain_Present, "IDXGISwapChain::Present");
	ResizeBuffersHook = addHook(vftable[13], SwapChain_ResizeBuffers, "IDXGISwapChain::ResizeBuffers");
	if (cqueueVftable) ExecuteCommandListsHook = addHook(cqueueVftable[10], CommandQueue_ExecuteCommandLists, "ID3D12CommandQueue::executeCommandLists");
}