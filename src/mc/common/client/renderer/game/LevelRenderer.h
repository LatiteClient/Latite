#pragma once
#include "mc/Util.h"
#include "LevelRendererPlayer.h"

namespace SDK {
    class LevelRenderer {
        CLASS_FIELD(LevelRendererPlayer*, levelRendererPlayer, 0x478);

    public:
        LevelRendererPlayer* getLevelRendererPlayer() { return levelRendererPlayer; }
    };
}
