#pragma once
#include "mc/Util.h"

namespace SDK {
    class ItemRenderer {
    public:
        void renderGuiItemNew(class BaseActorRenderContext* baseActorRenderContext, class ItemStack* itemStack, int mode, float x, float y, float opacity, float scale, float a9, bool ench);
    };
}