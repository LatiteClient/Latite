#include "pch.h"

#include "CustomSkinPickerHooks.h"

#include <commdlg.h>
#include <cstddef>
#include <vector>

#include "client/Latite.h"
#include "mc/Addresses.h"
#include "util/Logger.h"

namespace {
    std::shared_ptr<Hook> pickImageHook;

    bool isWindows10() noexcept {
        using RtlGetVersion = LONG(WINAPI*)(OSVERSIONINFOW*);

        const auto ntdll = GetModuleHandleW(L"ntdll.dll");
        const auto rtlGetVersion =
            ntdll ? reinterpret_cast<RtlGetVersion>(GetProcAddress(ntdll, "RtlGetVersion")) : nullptr;
        if (!rtlGetVersion) {
            return false;
        }

        OSVERSIONINFOW version {};
        version.dwOSVersionInfoSize = sizeof(version);
        return rtlGetVersion(&version) == 0 && version.dwMajorVersion == 10 && version.dwBuildNumber < 22000;
    }

    void queueCancelled(std::shared_ptr<SDK::ImagePickingCallback> callback) {
        Latite::get().queueForClientThread([callback = std::move(callback)] {
            callback->onImagePickCancelled();
        });
    }

    void queueSelected(std::shared_ptr<SDK::ImagePickingCallback> callback, std::string path) {
        Latite::get().queueForClientThread([callback = std::move(callback), path = std::move(path)] {
            SDK::Core::Path selectedPath { std::move(path) };
            callback->onImagePicked(selectedPath);
        });
    }

}

void CustomSkinPickerHooks::showLegacyPicker(void* appPlatform, std::shared_ptr<SDK::ImagePickingCallback> callback) {
    std::vector<wchar_t> selectedFile(32768);
    constexpr wchar_t pngFilter[] = L"PNG image (*.png)\0*.png\0\0";

    HWND owner = nullptr;
    if (appPlatform) {
        owner = *reinterpret_cast<HWND*>(reinterpret_cast<std::byte*>(appPlatform) + 0x780);
        if (!IsWindow(owner)) {
            owner = nullptr;
        }
    }

    OPENFILENAMEW dialog {};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = owner;
    dialog.lpstrFilter = pngFilter;
    dialog.lpstrFile = selectedFile.data();
    dialog.nMaxFile = static_cast<DWORD>(selectedFile.size());
    dialog.lpstrTitle = L"Choose a skin";
    dialog.lpstrDefExt = L"png";
    dialog.Flags =
        OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&dialog)) {
        queueSelected(std::move(callback), util::WStrToStr(std::wstring(selectedFile.data())));
        return;
    }

    const auto error = CommDlgExtendedError();
    if (error != 0) {
        Logger::Warn("Windows 10 custom-skin picker failed with common-dialog error 0x{:X}", error);
    }
    queueCancelled(std::move(callback));
}

void __fastcall CustomSkinPickerHooks::pickImage(void* appPlatform,
                                                 std::shared_ptr<SDK::ImagePickingCallback> callback) {
    if (!callback) {
        return;
    }

    auto fallbackCallback = callback;
    try {
        pickImageHook->oFunc<decltype(&pickImage)>()(appPlatform, std::move(callback));
        return;
    } catch (...) {
        Logger::Warn("Minecraft's Windows App SDK skin picker failed, opening the Windows 10 fallback picker...");
    }

    showLegacyPicker(appPlatform, std::move(fallbackCallback));
}

CustomSkinPickerHooks::CustomSkinPickerHooks()
    : HookGroup("Windows 10 custom-skin picker workaround") {
    // TODO(1.26.50): Remove this hook and the legacy picker implementation entirely.
    if (!Latite::get().tmp2640Is4240 || !isWindows10() ||
        !Latite::supportsMinecraftVersion(Latite::get().gameVersion) ||
        !Signatures::AppPlatform_GameCorePC_pickImage.result) {
        return;
    }

    pickImageHook =
        addHook(Signatures::AppPlatform_GameCorePC_pickImage.result, pickImage, "AppPlatform_GameCorePC::pickImage");
    Logger::Info("Installed Windows 10 custom-skin picker workaround.");
}
