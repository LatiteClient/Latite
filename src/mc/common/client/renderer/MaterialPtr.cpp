#include "pch.h"
#include "MaterialPtr.h"
#include "util/Util.h"

SDK::MaterialPtr* SDK::MaterialPtr::getUIColor() {
    static auto uiFillColorMaterial = createMaterial(HashedString("ui_fill_color"));
    return uiFillColorMaterial.get();
}

SDK::MaterialPtr* SDK::MaterialPtr::getUITextureAndColor() {
    static auto uiTexturedMaterial = createMaterial(HashedString("ui_textured"));
    return uiTexturedMaterial.get();
}

SDK::MaterialPtr* SDK::MaterialPtr::getSelectionBoxMaterial() {
    static auto material = SDK::MaterialPtr::createMaterial(HashedString("selection_box"));
    return material.get();
}

SDK::MaterialPtr* SDK::MaterialPtr::getSelectionOverlayMaterial() {
    static auto material = SDK::MaterialPtr::createMaterial(HashedString("selection_overlay"));
    return material.get();
}

std::shared_ptr<SDK::MaterialPtr> SDK::MaterialPtr::createMaterial(const SDK::HashedString& name) {
    static class RenderMaterialGroup* materialGroup =
        Signatures::RenderMaterialGroup__common.as_ptr<class RenderMaterialGroup>();

    std::shared_ptr<SDK::MaterialPtr> material;
    memory::callVirtual<void, std::shared_ptr<SDK::MaterialPtr>&, const SDK::HashedString&>(materialGroup, 1, material,
                                                                                            name);
    return material;
}
