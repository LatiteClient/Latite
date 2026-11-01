#include "pch.h"
#include "LevelRendererPlayer.h"
#include "mc/Addresses.h"
#include "util/LMath.h"

Vec3& SDK::LevelRendererPlayer::getOrigin() {
    return hat::member_at<Vec3>(this, 0x658);
}

float& SDK::LevelRendererPlayer::getFovX() {
    return hat::member_at<float>(this, 0xF50);
}

float& SDK::LevelRendererPlayer::getFovY() {
    return hat::member_at<float>(this, 0xF64);
}
