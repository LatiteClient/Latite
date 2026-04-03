#pragma once

namespace SDK {
    class GameCore { // Made up
    public:
        CLASS_FIELD(std::string, dataPath, 0x470);
        CLASS_FIELD(HWND, hwnd, 0x780);

        static GameCore* get();
    };
}