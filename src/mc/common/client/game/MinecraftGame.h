#pragma once
#include "mc/Util.h"
#include "util/memory.h"
#include <string>

#include "mc/common/client/renderer/GameRenderer.h"

namespace SDK {
    class MinecraftGame {
    public:
        bool isCursorGrabbed();

        CLASS_FIELD(GameRenderer*, gameRenderer, 0x1440);

        class FontRepository* getFontRepository();

        class ClientInstance* getPrimaryClientInstance();
    };
}
