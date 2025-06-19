#pragma once
#include <optional>
#include "LMath.h"

namespace WorldToScreen {
    std::optional<Vec2> convert(const Vec3& worldPos);
};