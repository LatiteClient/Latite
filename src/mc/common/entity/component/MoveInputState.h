#pragma once

namespace SDK {
    struct MoveInputState {
        bool sneakDown : 1;
        bool sneakToggleDown : 1;
        bool wantDownSlow : 1;
        bool wantUpSlow : 1;
        bool blockSelectDown : 1;
        bool ascendBlock : 1;
        bool descendBlock : 1;
        bool jumpDown : 1;
        bool sprintDown : 1;
        bool upLeft : 1;
        bool upRight : 1;
        bool downLeft : 1;
        bool downRight : 1;
        bool up : 1;
        bool down : 1;
        bool left : 1;
        bool right : 1;
        bool ascend : 1;
        bool descend : 1;
        bool changeHeight : 1;
        bool lookCenter : 1;
        bool sneakInputCurrentlyDown : 1;
        bool sneakInputWasReleased : 1;
        bool sneakInputWasPressed : 1;
        bool jumpInputWasReleased : 1;
        bool jumpInputWasPressed : 1;
        bool jumpInputCurrentlyDown : 1;
        Vec2 analogMoveVector;
        uint8_t lookSlightDirField;
        uint8_t lookNormalDirField;
        uint8_t lookSmoothDirField;
    };
}
