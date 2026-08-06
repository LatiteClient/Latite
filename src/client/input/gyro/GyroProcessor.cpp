#include "pch.h"
#include "GyroProcessor.h"

#include <algorithm>
#include <cmath>

namespace {
    constexpr int64_t MAX_SAMPLE_GAP_NANOS = 100'000'000;
    constexpr float NANOSECONDS_TO_SECONDS = 1.0e-9f;
    constexpr float DEGREES_PER_RADIAN = 180.f / pi_f;

    float nonNegativeFinite(float value) {
        return std::isfinite(value) && value > 0.f ? value : 0.f;
    }

    float smoothstep(float lowerEdge, float upperEdge, float value) {
        if (value <= lowerEdge) return 0.f;
        if (value >= upperEdge) return 1.f;
        float progress = (value - lowerEdge) / (upperEdge - lowerEdge);
        return progress * progress * (3.f - 2.f * progress);
    }

    Vec2 scale(Vec2 value, float amount) {
        return { value.x * amount, value.y * amount };
    }

    float sensitivityForSpeed(float speed, GyroProcessor::Settings const& settings) {
        if (!settings.dynamicSensitivity || settings.fastSensitivity == settings.horizontalSensitivity)
            return settings.horizontalSensitivity;

        float startSpeed = nonNegativeFinite(settings.accelerationStartSpeed);
        float fullSpeed = nonNegativeFinite(settings.accelerationFullSpeed);
        if (fullSpeed <= startSpeed) return settings.horizontalSensitivity;

        float progress = std::clamp((speed - startSpeed) / (fullSpeed - startSpeed), 0.f, 1.f);
        return std::lerp(settings.horizontalSensitivity, settings.fastSensitivity, progress);
    }

    Vec2 applySoftRadialDeadzone(Vec2 aimRate, float configuredCutoff, float configuredRecoveryWidth) {
        float cutoff = nonNegativeFinite(configuredCutoff);
        float recoveryWidth = nonNegativeFinite(configuredRecoveryWidth);
        if (cutoff == 0.f && recoveryWidth == 0.f) return aimRate;

        float width = recoveryWidth > 0.f ? recoveryWidth : std::max(cutoff, 1.f);
        float recoveryEnd = cutoff + width;
        float speed = aimRate.magnitude();
        if (speed <= cutoff) return {};
        if (speed >= recoveryEnd) return aimRate;

        float progress = (speed - cutoff) / width;
        float amount = progress * progress * (3.f - 2.f * progress);
        return scale(aimRate, amount);
    }
}

Vec2 GyroProcessor::applyLowSpeedSmoothing(Vec2 rawAimRate, float deltaSeconds, Settings const& settings) {
    float smoothingTimeSeconds = nonNegativeFinite(settings.smoothingTime) * 0.001f;
    float bypassSpeed = nonNegativeFinite(settings.smoothingBypassSpeed);
    if (smoothingTimeSeconds == 0.f || bypassSpeed == 0.f) {
        resetSmoothing();
        return rawAimRate;
    }

    if (!smoothingInitialized) {
        smoothedAimRate_ = rawAimRate;
        smoothingInitialized = true;
        return rawAimRate;
    }

    float alpha = 1.f - std::exp(-deltaSeconds / smoothingTimeSeconds);
    smoothedAimRate_ = smoothedAimRate_ + scale(rawAimRate - smoothedAimRate_, alpha);

    float rawSpeed = rawAimRate.magnitude();
    if (rawSpeed >= bypassSpeed) return rawAimRate;

    float fullSmoothingSpeed = 0.5f * bypassSpeed;
    if (rawSpeed <= fullSmoothingSpeed) return smoothedAimRate_;

    float rawBlend = smoothstep(fullSmoothingSpeed, bypassSpeed, rawSpeed);
    return smoothedAimRate_ + scale(rawAimRate - smoothedAimRate_, rawBlend);
}

Vec2 GyroProcessor::process(std::span<GyroSample const> samples, Settings const& settings) {
    Vec2 cameraDelta;

    for (auto const& sample : samples) {
        if (sample.timestampNanos <= 0) continue;
        if (previousTimestampNanos == 0) {
            previousTimestampNanos = sample.timestampNanos;
            continue;
        }

        int64_t deltaNanos = sample.timestampNanos - previousTimestampNanos;
        if (deltaNanos <= 0) {
            resetSmoothing();
            continue;
        }

        previousTimestampNanos = sample.timestampNanos;
        bool validAngularVelocity = std::isfinite(sample.angularVelocity.x) &&
                                    std::isfinite(sample.angularVelocity.y) && std::isfinite(sample.angularVelocity.z);
        if (deltaNanos > MAX_SAMPLE_GAP_NANOS || !validAngularVelocity) {
            resetSmoothing();
            continue;
        }

        Vec3 angularVelocity = sample.angularVelocity - calibrationBias_;
        Vec2 aimRate { -angularVelocity.x * DEGREES_PER_RADIAN, angularVelocity.y * DEGREES_PER_RADIAN };
        float deltaSeconds = static_cast<float>(deltaNanos) * NANOSECONDS_TO_SECONDS;
        aimRate = applyLowSpeedSmoothing(aimRate, deltaSeconds, settings);
        float physicalAimSpeed = aimRate.magnitude();
        aimRate = applySoftRadialDeadzone(aimRate, settings.deadzoneCutoff, settings.deadzoneRecoveryWidth);

        float horizontalSensitivity = sensitivityForSpeed(physicalAimSpeed, settings);
        float pitch = 0.f;
        if (settings.verticalSensitivity > 0.f) {
            float verticalSensitivity = settings.verticalSensitivity;
            if (settings.dynamicSensitivity && settings.horizontalSensitivity > 0.f) {
                verticalSensitivity *= horizontalSensitivity / settings.horizontalSensitivity;
            }
            pitch = aimRate.x * verticalSensitivity * deltaSeconds;
        }
        float yaw = aimRate.y * horizontalSensitivity * deltaSeconds;
        if (settings.invertVertical) pitch = -pitch;
        if (settings.invertHorizontal) yaw = -yaw;
        cameraDelta = cameraDelta + Vec2 { pitch, yaw };
    }
    return cameraDelta;
}

void GyroProcessor::reset() {
    previousTimestampNanos = 0;
    resetSmoothing();
}

void GyroProcessor::resetSmoothing() {
    smoothedAimRate_ = {};
    smoothingInitialized = false;
}

void GyroProcessor::setCalibrationBias(Vec3 const& bias) {
    if (std::isfinite(bias.x) && std::isfinite(bias.y) && std::isfinite(bias.z)) calibrationBias_ = bias;
}

Vec3 const& GyroProcessor::calibrationBias() const {
    return calibrationBias_;
}
