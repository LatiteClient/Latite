/* Copyright (C) Imrglop, All rights reserved
 * Copyright (C) Latite Client contributors, All rights reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited without the permission of the author.
 * Proprietary and confidential
 * Written by Imrglop <contact: latiteclientgithub@gmail.com>, 2023
 */

#pragma once
#include "../Hook.h"
#include "util/DXUtil.h"

class DXHooks : public HookGroup {
private:
	static HRESULT __stdcall SwapChain_Present(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags);
	static HRESULT __stdcall SwapChain_ResizeBuffers(IDXGISwapChain* chain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	static HRESULT __stdcall CommandQueue_ExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);
public:
	DXHooks();
};