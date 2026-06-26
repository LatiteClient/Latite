#pragma once

#include "MoveInputState.h"

namespace SDK {
    struct MoveInputComponent : IEntityComponent {
        static constexpr uint32_t type_hash = 0x018B1887;

        MoveInputState inputState;
        MoveInputState rawInputState;
        int8_t holdAutoJumpInWaterTicks;
        Vec2 move;
        Vec2 lookDelta;
        Vec2 interactDir;
        Vec3 displacement;
        Vec3 displacementDelta;
        Vec3 cameraOrientation;
        bool sneaking : 1;
        bool sprinting : 1;
        bool wantUp : 1;
        bool wantDown : 1;
        bool jumping : 1;
        bool autoJumpingInWater : 1;
        bool moveInputStateLocked : 1;
        bool persistSneak : 1;
        bool autoJumpEnabled : 1;
        bool isCameraRelativeMovementEnabled : 1;
        bool isRotControlledByMoveDirection : 1;
        std::array<bool, 2> isPaddling;
    };
}
