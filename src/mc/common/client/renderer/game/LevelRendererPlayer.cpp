#include "pch.h"
#include "LevelRendererPlayer.h"
#include "mc/Addresses.h"
#include "util/LMath.h"

Vec3& SDK::LevelRendererPlayer::getOrigin() {
    if (SDK::internalVers == SDK::V1_18_12) return hat::member_at<Vec3>(this, 0x504);
    return hat::member_at<Vec3>(this, Signatures::Offset::LevelRendererPlayer_origin.result);
}

float& SDK::LevelRendererPlayer::getFovX() {
    if (SDK::internalVers >= SDK::V1_26_0) return hat::member_at<float>(this, 0xFF0);
    if (SDK::internalVers >= SDK::V1_21_120) return hat::member_at<float>(this, 0xF80);
    if (SDK::internalVers >= SDK::V1_21_110) return hat::member_at<float>(this, 0xF88);
    if (SDK::internalVers >= SDK::V1_21_90) return hat::member_at<float>(this, 0x1020);
    if (SDK::internalVers >= SDK::V1_21_80) return hat::member_at<float>(this, 0xFC8);
    if (SDK::internalVers >= SDK::V1_21_70) return hat::member_at<float>(this, 0xF30);
    if (SDK::internalVers >= SDK::V1_21_60) return hat::member_at<float>(this, 0xEC8);
    if (SDK::internalVers >= SDK::V1_21_50) return hat::member_at<float>(this, 0xED0);
    if (SDK::internalVers >= SDK::V1_21_40) return hat::member_at<float>(this, 0xED8);
    if (SDK::internalVers >= SDK::V1_21_30) return hat::member_at<float>(this, 0xE28);
    if (SDK::internalVers >= SDK::V1_21) return hat::member_at<float>(this, 0xDF0);
    if (SDK::internalVers >= SDK::V1_20_80) return hat::member_at<float>(this, 0xDF8);
    if (SDK::internalVers >= SDK::V1_20_71) return hat::member_at<float>(this, 0xDD8);
    if (SDK::internalVers >= SDK::V1_20_50) return hat::member_at<float>(this, 0xDC0);
    if (SDK::internalVers == SDK::V1_18_12) return hat::member_at<float>(this, 0x1418);
    if (SDK::internalVers >= SDK::V1_20_30) return hat::member_at<float>(this, 0xDB8);
    return hat::member_at<float>(this, Signatures::Offset::LevelRendererPlayer_fovX.result);
}

float& SDK::LevelRendererPlayer::getFovY() {
    if (SDK::internalVers >= SDK::V1_26_0) return hat::member_at<float>(this, 0x1004);
    if (SDK::internalVers >= SDK::V1_21_120) return hat::member_at<float>(this, 0xF94);
    if (SDK::internalVers >= SDK::V1_21_110) return hat::member_at<float>(this, 0xF9C);
    if (SDK::internalVers >= SDK::V1_21_90) return hat::member_at<float>(this, 0x1034);
    if (SDK::internalVers >= SDK::V1_21_80) return hat::member_at<float>(this, 0xFDC);
    if (SDK::internalVers >= SDK::V1_21_70) return hat::member_at<float>(this, 0xF44);
    if (SDK::internalVers >= SDK::V1_21_60) return hat::member_at<float>(this, 0xEDC);
    if (SDK::internalVers >= SDK::V1_21_50) return hat::member_at<float>(this, 0xEE4);
    if (SDK::internalVers >= SDK::V1_21_40) return hat::member_at<float>(this, 0xEEC);
    if (SDK::internalVers >= SDK::V1_21_30) return hat::member_at<float>(this, 0xE3C);
    if (SDK::internalVers >= SDK::V1_21) return hat::member_at<float>(this, 0xE04);
    if (SDK::internalVers >= SDK::V1_20_80) return hat::member_at<float>(this, 0xE0C);
    if (SDK::internalVers >= SDK::V1_20_71) return hat::member_at<float>(this, 0xDEC);
    if (SDK::internalVers >= SDK::V1_20_50) return hat::member_at<float>(this, 0xDD4);
    if (SDK::internalVers == SDK::V1_18_12) return hat::member_at<float>(this, 0x142C);
    if (SDK::internalVers >= SDK::V1_20_30) return hat::member_at<float>(this, 0xDCC);
    return hat::member_at<float>(this, Signatures::Offset::LevelRendererPlayer_fovX.result + 20);
}
