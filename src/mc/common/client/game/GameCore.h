#pragma once

namespace SDK {
    class GameCore { // Made up
    public:
        CLASS_FIELD(std::string, dataPath, 0x480);
        CLASS_FIELD(HWND, hwnd, 0x770);

        static GameCore* get();
    };
}