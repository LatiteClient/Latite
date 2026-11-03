#pragma once

namespace SDK {
    class GameCore { // Made up
    public:
        CLASS_FIELD(std::string, dataPath, 0x90);
        CLASS_FIELD(HWND, hwnd, 0x3A0);

        static GameCore* get();
    };
}
