#include "pch.h"
#include "MaterialPtr.h"
#include "util/Util.h"

SDK::MaterialPtr* SDK::MaterialPtr::getUIColor() {
    static auto uiFillColorMaterial = createMaterial(HashedString("ui_fill_color"));
    return uiFillColorMaterial;
}

SDK::MaterialPtr* SDK::MaterialPtr::getUITextureAndColor() {
    static auto uiTexturedMaterial = createMaterial(HashedString("ui_textured"));
    return uiTexturedMaterial;
}

SDK::MaterialPtr* SDK::MaterialPtr::getSelectionBoxMaterial() {
    static auto material = SDK::MaterialPtr::createMaterial(HashedString("selection_box"));
    return material;
};
SDK::MaterialPtr* SDK::MaterialPtr::getSelectionOverlayMaterial() {
    static auto material = SDK::MaterialPtr::createMaterial(HashedString("selection_overlay"));
    return material;
};

// TODO: Replace SDK::MaterialPtr* return value with std::shared_ptr<SDK::MaterialPtr> in the next update
SDK::MaterialPtr* SDK::MaterialPtr::createMaterial(const SDK::HashedString& name) {
    static class RenderMaterialGroup* materialGroup =
        Signatures::RenderMaterialGroup__common.as_ptr<class RenderMaterialGroup>();

    if (Latite::get().tmp2640Is4240)
        return memory::callVirtual<SDK::MaterialPtr*, const SDK::HashedString&>(materialGroup, 1, name);

    std::shared_ptr<SDK::MaterialPtr> buf{};
    memory::callVirtual<void, std::shared_ptr<SDK::MaterialPtr>&, const SDK::HashedString&>(materialGroup, 1, buf, name);
    return buf.get();
}
