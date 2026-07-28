#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>

#include "GyroProcessor.h"

struct AccelerometerSample final {
    Vec3 acceleration; // meters per second squared, including gravity
    int64_t timestampNanos = 0;
};

class GyroCalibration final {
public:
    enum class Mode {
        Calibration,
        Verification,
        Diagnostics
    };

    enum class FailureReason {
        None,
        InsufficientSamples,
        InsufficientAccelerometerSamples,
        MotionDetected,
        BiasUnstable
    };

    struct Result final {
        FailureReason failureReason = FailureReason::None;
        Vec3 bias;
        Vec3 standardDeviation;
        Vec3 peakAngularDeviation;
        Vec3 maximumBlockMeanDelta;
        Vec3 meanAcceleration;
        float peakAngularDeviationMagnitude = 0.f;
        float accelerometerRmsDeviation = 0.f;
        float meanGravityError = 0.f;
        float peakAccelerationMagnitudeError = 0.f;
        float maximumAccelerationBlockMeanDelta = 0.f;
        float standardDeviationLimit = 0.f;
        float peakAngularDeviationLimit = 0.f;
        float blockMeanDeltaLimit = 0.f;
        float accelerometerRmsDeviationLimit = 0.f;
        float meanGravityErrorLimit = 0.f;
        float peakAccelerationMagnitudeErrorLimit = 0.f;
        float accelerationBlockMeanDeltaLimit = 0.f;
        float sampleRate = 0.f;
        float accelerometerSampleRate = 0.f;
        double measurementDurationSeconds = 0.0;
        double accelerometerMeasurementDurationSeconds = 0.0;
        std::size_t sampleCount = 0;
        std::size_t receivedGyroSampleCount = 0;
        std::size_t rejectedGyroSampleCount = 0;
        std::size_t rejectedGyroSampleLimit = 0;
        std::size_t accelerometerSampleCount = 0;
        bool accelerometerAssisted = false;

        [[nodiscard]] bool successful() const { return failureReason == FailureReason::None; }
    };

    [[nodiscard]] std::chrono::milliseconds begin(Mode mode, bool accelerometerAssisted);
    void cancel();
    [[nodiscard]] std::optional<Result> addSample(GyroSample const& sample);
    void addAccelerometerSample(AccelerometerSample const& sample);

private:
    struct BlockMeasurement {
        double weightedAngularVelocityX = 0.0;
        double weightedAngularVelocityY = 0.0;
        double weightedAngularVelocityZ = 0.0;
        double durationSeconds = 0.0;
    };

    struct AccelerometerBlockMeasurement {
        double weightedAccelerationX = 0.0;
        double weightedAccelerationY = 0.0;
        double weightedAccelerationZ = 0.0;
        double durationSeconds = 0.0;
    };

    static constexpr std::size_t BLOCK_CAPACITY = 8;

    void resetMeasurements();
    void accumulate(GyroSample const& sample);
    void accumulateAccelerometer(AccelerometerSample const& sample);
    [[nodiscard]] Result finish() const;

    bool active_ = false;
    bool accelerometerAssisted_ = false;
    int64_t settlingDurationNanos_ = 0;
    int64_t collectionDurationNanos_ = 0;
    std::size_t minimumSampleCount_ = 0;
    std::size_t minimumAccelerometerSampleCount_ = 0;
    int64_t firstTimestampNanos_ = 0;
    int64_t previousTimestampNanos_ = 0;
    int64_t collectionStartNanos_ = 0;
    int64_t firstCollectedTimestampNanos_ = 0;
    int64_t lastCollectedTimestampNanos_ = 0;
    std::size_t sampleCount_ = 0;
    std::size_t receivedGyroSampleCount_ = 0;
    std::size_t rejectedGyroSampleCount_ = 0;
    double meanX_ = 0.0, meanY_ = 0.0, meanZ_ = 0.0;
    double sumSquaredDifferencesX_ = 0.0, sumSquaredDifferencesY_ = 0.0, sumSquaredDifferencesZ_ = 0.0;
    double weightedAngularVelocityX_ = 0.0, weightedAngularVelocityY_ = 0.0, weightedAngularVelocityZ_ = 0.0;
    double weightedAngularVelocityCorrectionX_ = 0.0;
    double weightedAngularVelocityCorrectionY_ = 0.0;
    double weightedAngularVelocityCorrectionZ_ = 0.0;
    double weightedDurationSeconds_ = 0.0;
    std::array<BlockMeasurement, BLOCK_CAPACITY> blocks_;
    std::size_t blockCount_ = 0;
    Vec3 minimumAngularVelocity_;
    Vec3 maximumAngularVelocity_;
    bool angularVelocityExtentsInitialized_ = false;

    int64_t previousAccelerometerTimestampNanos_ = 0;
    int64_t firstAccelerometerTimestampNanos_ = 0;
    int64_t lastAccelerometerTimestampNanos_ = 0;
    std::size_t accelerometerSampleCount_ = 0;
    double meanAccelerationX_ = 0.0, meanAccelerationY_ = 0.0, meanAccelerationZ_ = 0.0;
    double sumSquaredAccelerationDifferencesX_ = 0.0;
    double sumSquaredAccelerationDifferencesY_ = 0.0;
    double sumSquaredAccelerationDifferencesZ_ = 0.0;
    double weightedAccelerationX_ = 0.0, weightedAccelerationY_ = 0.0, weightedAccelerationZ_ = 0.0;
    double weightedAccelerationCorrectionX_ = 0.0;
    double weightedAccelerationCorrectionY_ = 0.0;
    double weightedAccelerationCorrectionZ_ = 0.0;
    double weightedAccelerationDurationSeconds_ = 0.0;
    std::array<AccelerometerBlockMeasurement, BLOCK_CAPACITY> accelerometerBlocks_;
    std::size_t accelerometerBlockCount_ = 0;
    float peakAccelerationMagnitudeError_ = 0.f;
};
