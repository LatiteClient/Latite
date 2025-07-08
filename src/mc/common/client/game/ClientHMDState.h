#pragma once
#include "mc/common/client/renderer/Matrix.h"

namespace SDK {
    class ClientHMDState {
        uint8_t pad_8[0xC8];
    public:
        // mLastLevelViewMatrix | Offset: 0x0C8 (200)
        Matrix lastViewMatrix;

        // mLastLevelViewMatrixAbsolute | Offset: 0x108 (264)
        Matrix lastViewMatrixAbsolute;

        // mLastLevelProjMatrix | Offset: 0x148 (328)
        Matrix lastProjectionMatrix;

        // mLastLevelWorldMatrix | Offset: 0x188 (392)
        Matrix lastWorldMatrix;

        // mHUDMatrixPatch | Offset: 0x1C8 (456)
        Matrix hudMatrixPatch;

        // mVRTransitionMatrixPatch | Offset: 0x208 (520)
        Matrix vrTransitionMatrixPatch;
    };
}
