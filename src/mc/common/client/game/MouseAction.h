#pragma once

namespace SDK {
    struct MouseAction {
        int16_t x;
        int16_t y;
        int16_t dx;
        int16_t dy;
        int8_t action;
        int8_t data;
        int pointerId;
        bool forceMotionlessPointer;
    };
}
