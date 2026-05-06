#include "pch.h"
#include "LevelRendererPlayer.h"
#include "mc/Addresses.h"
#include "util/LMath.h"

Vec3& SDK::LevelRendererPlayer::getOrigin() {
    return hat::member_at<Vec3>(this, Signatures::Offset::LevelRendererPlayer_origin.result);
}

float& SDK::LevelRendererPlayer::getFovX() {
    return hat::member_at<float>(this, 0xFF0);
}

float& SDK::LevelRendererPlayer::getFovY() {
    return hat::member_at<float>(this, 0x1004);
}
