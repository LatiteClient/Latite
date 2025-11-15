#pragma once

namespace SDK {
    class GameCore { // Made up
    public:
        CLASS_FIELD(HWND, hwnd, 0x770);
    
        static GameCore* get();
    };
}
