#include "LevelRendererPlayer.h"
#include "sdk/signature/storage.h"
#include "sdk/util.h"
#include "util/LMath.h"

Vec3& sdk::LevelRendererPlayer::getOrigin() {
    if (sdk::internalVers == sdk::V1_18_12) return util::directAccess<Vec3>(this, 0x504);
    return util::directAccess<Vec3>(this, Signatures::Offset::LevelRendererPlayer_origin.result);
}

float& sdk::LevelRendererPlayer::getFovX() {
    if (sdk::internalVers == sdk::V1_18_12) return util::directAccess<float>(this, 0x1418);
    return util::directAccess<float>(this, Signatures::Offset::LevelRendererPlayer_fovX.result);
}

float& sdk::LevelRendererPlayer::getFovY() {
    if (sdk::internalVers == sdk::V1_18_12) return util::directAccess<float>(this, 0x142C);
    return util::directAccess<float>(this, Signatures::Offset::LevelRendererPlayer_fovX.result + 20);
}
