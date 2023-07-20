#include "LevelRendererPlayer.h"
#include "sdk/signature/storage.h"
#include "sdk/util.h"
#include "util/LMath.h"

Vec3& sdk::LevelRendererPlayer::getOrigin()
{
    return util::directAccess<Vec3>(this, Signatures::Offset::LevelRendererPlayer_origin.result);
}

float& sdk::LevelRendererPlayer::getFovX()
{
    return util::directAccess<float>(this, Signatures::Offset::LevelRendererPlayer_fovX.result);
}

float& sdk::LevelRendererPlayer::getFovY()
{
    return util::directAccess<float>(this, Signatures::Offset::LevelRendererPlayer_fovX.result + 20);
}
