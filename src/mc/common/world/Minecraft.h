#pragma once
#include "level/Level.h"
#include "mc/Util.h"
#include "Timer.h"

namespace SDK {
    class Minecraft {
    public:
        Level* getLevel();

        CLASS_FIELD(class GameSession*, gameSession, 0xC0);
        CLASS_FIELD(Timer*, timer, 0xD8);
    };

    class GameSession {
    public:
        Level* getLevel();
    };
}
