#pragma once

#include "client/memory/hook/Hook.h"
#include "mc/deps/application/ImagePickingCallback.h"

class CustomSkinPickerHooks : public HookGroup {
private:
    static void showLegacyPicker(void* appPlatform, std::shared_ptr<SDK::ImagePickingCallback> callback);
    static void __fastcall pickImage(void* appPlatform, std::shared_ptr<SDK::ImagePickingCallback> callback);

public:
    CustomSkinPickerHooks();
};
