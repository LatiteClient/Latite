#pragma once
#include "../Hook.h"
#include "util/DXUtil.h"

class DXHooks : public HookGroup {
private:
	static HRESULT __stdcall SwapChain_Present(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags);
	static HRESULT __stdcall SwapChain_ResizeBuffers(IDXGISwapChain* chain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
public:
	DXHooks();
};