#pragma once

namespace SDK {
    class GameCore { // Made up
    public:
        CLASS_FIELD(std::string, dataPath, 0x498);
        CLASS_FIELD(HWND, hwnd, 0x788);

        static GameCore* get();
    };
}