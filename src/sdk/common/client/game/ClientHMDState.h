#pragma once
#include "sdk/common/client/renderer/MinecraftMatrix.h"

namespace SDK {
    struct ClientHMDState {
        uint8_t pad_8[0xC8];

        // mLastLevelViewMatrix | Offset: 0x0C8 (200)
        SDK::MinecraftMatrix lastViewMatrix;

        // mLastLevelViewMatrixAbsolute | Offset: 0x108 (264)
        SDK::MinecraftMatrix lastViewMatrixAbsolute;

        // mLastLevelProjMatrix | Offset: 0x148 (328)
        SDK::MinecraftMatrix lastProjectionMatrix;

        // mLastLevelWorldMatrix | Offset: 0x188 (392)
        SDK::MinecraftMatrix lastWorldMatrix;

        // mHUDMatrixPatch | Offset: 0x1C8 (456)
        SDK::MinecraftMatrix hudMatrixPatch;

        // mVRTransitionMatrixPatch | Offset: 0x208 (520)
        SDK::MinecraftMatrix vrTransitionMatrixPatch;
    };
}
