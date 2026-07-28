#pragma once

#include <cstdint>
#include <span>

#include "util/LMath.h"

struct GyroSample final {
    Vec3 angularVelocity;
    int64_t timestampNanos = 0;
};

class GyroProcessor final {
public:
    struct Settings final {
        float sensitivity = 1.f;
        float verticalRatio = 1.f;
        float fastSensitivity = 1.f;
        float accelerationStartSpeed = 20.f;
        float accelerationFullSpeed = 120.f;
        float deadzoneCutoff = 0.f;
        float deadzoneRecoveryWidth = 0.f;
        float smoothingTime = 0.f;
        float smoothingBypassSpeed = 0.f;
        bool dynamicSensitivity = false;
        bool invertHorizontal = false;
        bool invertVertical = false;
    };

    [[nodiscard]] Vec2 process(std::span<GyroSample const> samples, Settings const& settings);
    void reset();
    void setCalibrationBias(Vec3 const& bias);
    [[nodiscard]] Vec3 const& calibrationBias() const;

private:
    [[nodiscard]] Vec2 applyLowSpeedSmoothing(Vec2 rawAimRate, float deltaSeconds, Settings const& settings);
    void resetSmoothing();

    int64_t previousTimestampNanos = 0;
    Vec3 calibrationBias_;
    Vec2 smoothedAimRate_;
    bool smoothingInitialized = false;
};
