#pragma once
#include "../Hook.h"
#include "util/DXUtil.h"

class DXHooks : public HookGroup {
private:
	static HRESULT WINAPI CreateSwapChainForCoreWindowHook(IDXGIFactory2* pFactory, IUnknown* pDevice, IUnknown* pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain);
	static HRESULT __stdcall SwapChain_Present(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags);
	static HRESULT __stdcall SwapChain_ResizeBuffers(IDXGISwapChain* chain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	static HRESULT __stdcall CommandQueue_ExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);

	static void CheckTearingSupport();
public:
	DXHooks();
};