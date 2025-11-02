#include "pch.h"
#include "DXHooks.h"
#include "client/Latite.h"
#include "client/render/Renderer.h"
#include "sdk/common/client/game/Options.h"

namespace {
    typedef HRESULT(WINAPI* CreateSwapChainForHWND_t)(
        IDXGIFactory2*,
        IUnknown*,
        HWND,
        const DXGI_SWAP_CHAIN_DESC1*,
        const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*,
        IDXGIOutput*,
        IDXGISwapChain1**);

    bool tearingSupported = false;
    bool isForceDisableVSync = false;
    std::shared_ptr<Hook> PresentHook;
    std::shared_ptr<Hook> ResizeBuffersHook;
    std::shared_ptr<Hook> ExecuteCommandListsHook;

    // TODO: temporary, remove this
    bool isGfxVsyncDisabled() {
        wchar_t appdata[MAX_PATH];
        DWORD pathLen = GetEnvironmentVariableW(L"APPDATA", appdata, MAX_PATH);
        if (pathLen == 0 || pathLen >= MAX_PATH) {
            return true;
        }

        std::wstring optionsPath(appdata);
        optionsPath +=
            L"\\Minecraft Bedrock\\Users\\" + util::StrToWStr(SDK::ClientInstance::get()->minecraftGame->xuid) + L"\\games\\com.mojang\\minecraftpe\\options.txt";

        if (!std::filesystem::exists(optionsPath))
            optionsPath = std::wstring(appdata) + L"\\Minecraft Bedrock\\Users\\Shared\\games\\com.mojang\\minecraftpe\\options.txt";

        std::ifstream file(optionsPath.c_str());
        if (!file.is_open()) {
            return true;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::erase(line, '\r');
            if (line.find("gfx_vsync:") == 0) {
                return line != "gfx_vsync:0";
            }
        }

        // default to enabled
        return true;
    }
}

void DXHooks::CheckForceDisableVSync() {
    if (Latite::get().shouldForceDisableVSync()) {
        isForceDisableVSync = true;
    } else {
        isForceDisableVSync = false;
    }
}

void DXHooks::CheckTearingSupport() {
    ComPtr<IDXGIFactory5> factory5;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory5)))) {
        BOOL allowTearing = FALSE;
        if (SUCCEEDED(factory5->CheckFeatureSupport(
            DXGI_FEATURE_PRESENT_ALLOW_TEARING,
            &allowTearing,
            sizeof(allowTearing)))) {
            if (allowTearing && !isGfxVsyncDisabled()) {
                tearingSupported = true;
            }
        }
    }
}

CreateSwapChainForHWND_t origCreateSwapChain = nullptr;

HRESULT WINAPI DXHooks::CreateSwapChainForHWNDHook(
    IDXGIFactory2* factory,
    IUnknown* device,
    HWND hwnd,
    const DXGI_SWAP_CHAIN_DESC1* desc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
    IDXGIOutput* output,
    IDXGISwapChain1** swapChain) {

    DXGI_SWAP_CHAIN_DESC1 modifiedDesc = *desc;
    if (tearingSupported && isForceDisableVSync) {
        modifiedDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    }

    return origCreateSwapChain(factory, device, hwnd, &modifiedDesc, pFullscreenDesc,
        output, swapChain);
}

HRESULT __stdcall DXHooks::SwapChain_Present(IDXGISwapChain* chain, UINT SyncInterval, UINT Flags) {
    if (Latite::getRenderer().hasInitialized()) {
        auto lock = Latite::getRenderer().lock();
        Latite::getRenderer().render();
    }

    if (!Latite::getRenderer().hasInitialized()) {
        auto lock = Latite::getRenderer().lock();
        Latite::getRenderer().init(chain);
    }

    //static bool hasKilled = false;
    //if (!hasKilled) {
    //	ComPtr<ID3D12Device> d3d12Device;
    //	chain->GetDevice(IID_PPV_ARGS(&d3d12Device));
    //	if (d3d12Device) ((ID3D12Device5*)d3d12Device.Get())->RemoveDevice(); // kill dx
    //	hasKilled = true;
    //}

    UINT presentFlags = Flags;
    UINT syncInterval = SyncInterval;
    if (tearingSupported && isForceDisableVSync) {
        syncInterval = 0;
        presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
    }

    return PresentHook->oFunc<decltype(&SwapChain_Present)>()(chain, syncInterval, presentFlags);
}

HRESULT __stdcall DXHooks::SwapChain_ResizeBuffers(
    IDXGISwapChain* chain,
    UINT BufferCount,
    UINT Width,
    UINT Height,
    DXGI_FORMAT NewFormat,
    UINT SwapChainFlags,
    const UINT *pCreationNodeMask,
    IUnknown *const *ppPresentQueue) {

    Latite::getRenderer().reinit();
    UINT newFlags = SwapChainFlags;
    if (tearingSupported && isForceDisableVSync) {
        newFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    }

    return ResizeBuffersHook->oFunc<decltype(&SwapChain_ResizeBuffers)>()(
        chain, BufferCount, Width, Height, NewFormat, newFlags, pCreationNodeMask, ppPresentQueue);
}

HRESULT __stdcall DXHooks::CommandQueue_ExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists,
    ID3D12CommandList* const* ppCommandLists) {
    auto lock = Latite::getRenderer().lock();
    Latite::getRenderer().setCommandQueue(queue);
    return ExecuteCommandListsHook->oFunc<decltype(&CommandQueue_ExecuteCommandLists)>()(
        queue, NumCommandLists, ppCommandLists);
}

DXHooks::DXHooks() : HookGroup("DirectX") {
    ComPtr<IDXGIFactory> factory;
    ComPtr<IDXGISwapChain> swapChain;
    ComPtr<IDXGIAdapter> adapter;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D12Device> device12;
    ComPtr<ID3D11DeviceContext> dctx;
    ComPtr<ID3D12CommandQueue> cqueue;

    DXHooks::CheckForceDisableVSync();
    DXHooks::CheckTearingSupport();

    ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&factory)));
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
    auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, lvl, 2, D3D11_SDK_VERSION,
        &swapChainDesc, swapChain.GetAddressOf(), device.GetAddressOf(),
        &featureLevel, dctx.GetAddressOf());

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

    ComPtr<IDXGIFactory2> factory2;
    if (SUCCEEDED(factory.As(&factory2))) {
        void** vtable = *(void***)factory2.Get();
        MH_CreateHook(vtable[15], DXHooks::CreateSwapChainForHWNDHook,
            (void**)&origCreateSwapChain);
        MH_EnableHook(vtable[15]);
    }

    PresentHook = addHook(vftable[8], SwapChain_Present, "IDXGISwapChain::Present");
    ResizeBuffersHook = addHook(vftable[39], SwapChain_ResizeBuffers, "IDXGISwapChain3::ResizeBuffers");

    // Needed for D3D11On12 for DX12
    if (cqueueVftable) ExecuteCommandListsHook = addHook(cqueueVftable[10], CommandQueue_ExecuteCommandLists, "ID3D12CommandQueue::executeCommandLists");
    PresentHook->enable();
    ResizeBuffersHook->enable();
    if (cqueueVftable) ExecuteCommandListsHook->enable();
}