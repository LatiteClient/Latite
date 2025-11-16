#pragma once
#include "Matrix.h"

namespace SDK {
    class GameRenderer {
        char pad[0x350];

    public:
        Matrix lastViewMatrix;

    private:
        char pad2[0x40];

    public:
        Matrix lastProjectionMatrix;
    };
}
