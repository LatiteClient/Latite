#include "pch.h"

void SDK::LocalPlayer::applyTurnDelta(const Vec2& delta) {
	reinterpret_cast<void(*)(LocalPlayer*, const Vec2&)>(Signatures::LocalPlayer_applyTurnDelta.result)(this, delta);
}
