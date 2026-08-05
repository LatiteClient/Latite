#pragma once

namespace SDK {
    class GameCore { // Made up
    public:
        CLASS_FIELD(std::string, dataPath, 0x470);
        CLASS_FIELD(HWND, hwnd, 0x7E0);

        static GameCore* get();
    };
}
