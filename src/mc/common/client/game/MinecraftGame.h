#pragma once
#include "mc/Util.h"
#include "util/memory.h"
#include <string>

#include "mc/common/client/renderer/GameRenderer.h"

namespace SDK {
    class MinecraftGame {
    public:
        bool isCursorGrabbed();

        CLASS_FIELD(std::string, xuid, 0x250);
        CLASS_FIELD(GameRenderer*, gameRenderer, 0xF18);

    private:
        CLASS_FIELD(class FontRepository**, fontRepo, 0x700);

    public:
        class FontRepository* getFontRepository() { return reinterpret_cast<FontRepository*>(fontRepo); }

        class ClientInstance* getPrimaryClientInstance();
    };
}
