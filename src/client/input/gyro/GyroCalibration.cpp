#include "pch.h"
#include "GyroCalibration.h"

#include <algorithm>
#include <cmath>

namespace {
    constexpr int64_t MAXIMUM_SAMPLE_GAP_NANOS = 100'000'000;
    constexpr int64_t BLOCK_DURATION_NANOS = 1'000'000'000;
    constexpr double MINIMUM_BLOCK_DURATION_SECONDS = 0.25;
    constexpr float EARTH_GRAVITY_METERS_PER_SECOND_SQUARED = 9.80665f;
    constexpr double NANOSECONDS_TO_SECONDS = 1.0e-9;
    constexpr std::size_t MINIMUM_TRANSIENT_BASELINE_SAMPLES = 16;
    constexpr std::size_t MINIMUM_REJECTED_GYRO_SAMPLE_LIMIT = 3;
    constexpr std::size_t REJECTED_GYRO_SAMPLE_LIMIT_DIVISOR = 100;
    constexpr float TRANSIENT_GYRO_DEVIATION_LIMIT = LatiteMath::deg2rad(5.f);

    void addCompensated(double value, double& sum, double& correction) {
        double adjustedValue = value - correction;
        double nextSum = sum + adjustedValue;
        correction = (nextSum - sum) - adjustedValue;
        sum = nextSum;
    }
}

std::chrono::milliseconds GyroCalibration::begin(Mode mode, bool accelerometerAssisted) {
    active_ = true;
    accelerometerAssisted_ = accelerometerAssisted;
    switch (mode) {
    case Mode::Calibration:
        settlingDurationNanos_ = 1'000'000'000;
        collectionDurationNanos_ = 5'000'000'000;
        minimumSampleCount_ = 150;
        minimumAccelerometerSampleCount_ = 75;
        break;
    case Mode::Verification:
        settlingDurationNanos_ = 250'000'000;
        collectionDurationNanos_ = 4'000'000'000;
        minimumSampleCount_ = 120;
        minimumAccelerometerSampleCount_ = 60;
        break;
    case Mode::Diagnostics:
        settlingDurationNanos_ = 250'000'000;
        collectionDurationNanos_ = 1'500'000'000;
        minimumSampleCount_ = 50;
        minimumAccelerometerSampleCount_ = 25;
        break;
    }
    resetMeasurements();
    auto totalDuration = std::chrono::nanoseconds(settlingDurationNanos_ + collectionDurationNanos_);
    return std::chrono::duration_cast<std::chrono::milliseconds>(totalDuration);
}

void GyroCalibration::cancel() {
    active_ = false;
    resetMeasurements();
}

std::optional<GyroCalibration::Result> GyroCalibration::addSample(GyroSample const& sample) {
    bool valid = std::isfinite(sample.angularVelocity.x) && std::isfinite(sample.angularVelocity.y) &&
                 std::isfinite(sample.angularVelocity.z);
    if (!active_ || sample.timestampNanos <= 0 || !valid) return std::nullopt;

    if (firstTimestampNanos_ == 0) {
        firstTimestampNanos_ = previousTimestampNanos_ = sample.timestampNanos;
        return std::nullopt;
    }
    if (sample.timestampNanos <= previousTimestampNanos_) return std::nullopt;
    previousTimestampNanos_ = sample.timestampNanos;

    if (collectionStartNanos_ == 0) {
        if (sample.timestampNanos - firstTimestampNanos_ < settlingDurationNanos_) return std::nullopt;
        collectionStartNanos_ = sample.timestampNanos;
    }

    ++receivedGyroSampleCount_;
    bool rejectTransient = false;

    if (sampleCount_ >= MINIMUM_TRANSIENT_BASELINE_SAMPLES) {
        float deviationX = sample.angularVelocity.x - static_cast<float>(meanX_);
        float deviationY = sample.angularVelocity.y - static_cast<float>(meanY_);
        float deviationZ = sample.angularVelocity.z - static_cast<float>(meanZ_);
        float deviationSquared = deviationX * deviationX + deviationY * deviationY + deviationZ * deviationZ;
        rejectTransient = deviationSquared > TRANSIENT_GYRO_DEVIATION_LIMIT * TRANSIENT_GYRO_DEVIATION_LIMIT;
    }
    if (rejectTransient)
        ++rejectedGyroSampleCount_;
    else
        accumulate(sample);

    if (sample.timestampNanos - collectionStartNanos_ < collectionDurationNanos_) return std::nullopt;
    active_ = false;
    return finish();
}

void GyroCalibration::addAccelerometerSample(AccelerometerSample const& sample) {
    bool valid = std::isfinite(sample.acceleration.x) && std::isfinite(sample.acceleration.y) &&
                 std::isfinite(sample.acceleration.z);
    if (!active_ || !accelerometerAssisted_ || collectionStartNanos_ == 0 ||
        sample.timestampNanos < collectionStartNanos_ || !valid)
        return;
    if (sample.timestampNanos > collectionStartNanos_ + collectionDurationNanos_) return;
    if (previousAccelerometerTimestampNanos_ != 0 && sample.timestampNanos <= previousAccelerometerTimestampNanos_)
        return;
    accumulateAccelerometer(sample);
}

void GyroCalibration::resetMeasurements() {
    firstTimestampNanos_ = previousTimestampNanos_ = collectionStartNanos_ = 0;
    firstCollectedTimestampNanos_ = lastCollectedTimestampNanos_ = 0;
    sampleCount_ = 0;
    receivedGyroSampleCount_ = 0;
    rejectedGyroSampleCount_ = 0;
    meanX_ = meanY_ = meanZ_ = 0.0;
    sumSquaredDifferencesX_ = sumSquaredDifferencesY_ = sumSquaredDifferencesZ_ = 0.0;
    weightedAngularVelocityX_ = weightedAngularVelocityY_ = weightedAngularVelocityZ_ = 0.0;
    weightedAngularVelocityCorrectionX_ = 0.0;
    weightedAngularVelocityCorrectionY_ = 0.0;
    weightedAngularVelocityCorrectionZ_ = 0.0;
    weightedDurationSeconds_ = 0.0;
    blocks_ = {};
    blockCount_ = 0;
    minimumAngularVelocity_ = {};
    maximumAngularVelocity_ = {};
    angularVelocityExtentsInitialized_ = false;

    previousAccelerometerTimestampNanos_ = firstAccelerometerTimestampNanos_ = lastAccelerometerTimestampNanos_ = 0;
    accelerometerSampleCount_ = 0;
    meanAccelerationX_ = meanAccelerationY_ = meanAccelerationZ_ = 0.0;
    sumSquaredAccelerationDifferencesX_ = 0.0;
    sumSquaredAccelerationDifferencesY_ = 0.0;
    sumSquaredAccelerationDifferencesZ_ = 0.0;
    weightedAccelerationX_ = weightedAccelerationY_ = weightedAccelerationZ_ = 0.0;
    weightedAccelerationCorrectionX_ = weightedAccelerationCorrectionY_ = weightedAccelerationCorrectionZ_ = 0.0;
    weightedAccelerationDurationSeconds_ = 0.0;
    accelerometerBlocks_ = {};
    accelerometerBlockCount_ = 0;
    peakAccelerationMagnitudeError_ = 0.f;
}

void GyroCalibration::accumulate(GyroSample const& sample) {
    int64_t previousCollectedTimestampNanos = lastCollectedTimestampNanos_;
    if (sampleCount_ == 0) {
        firstCollectedTimestampNanos_ = sample.timestampNanos;
    } else {
        int64_t deltaNanos = sample.timestampNanos - previousCollectedTimestampNanos;
        if (deltaNanos > 0 && deltaNanos <= MAXIMUM_SAMPLE_GAP_NANOS) {
            double deltaSeconds = static_cast<double>(deltaNanos) * NANOSECONDS_TO_SECONDS;
            addCompensated(static_cast<double>(sample.angularVelocity.x) * deltaSeconds, weightedAngularVelocityX_,
                           weightedAngularVelocityCorrectionX_);
            addCompensated(static_cast<double>(sample.angularVelocity.y) * deltaSeconds, weightedAngularVelocityY_,
                           weightedAngularVelocityCorrectionY_);
            addCompensated(static_cast<double>(sample.angularVelocity.z) * deltaSeconds, weightedAngularVelocityZ_,
                           weightedAngularVelocityCorrectionZ_);
            weightedDurationSeconds_ += deltaSeconds;

            int64_t intervalMidpointNanos = previousCollectedTimestampNanos + deltaNanos / 2;
            int64_t elapsedNanos = std::max(int64_t { 0 }, intervalMidpointNanos - collectionStartNanos_);
            std::size_t elapsedBlocks = static_cast<std::size_t>(elapsedNanos / BLOCK_DURATION_NANOS);
            std::size_t blockIndex = std::min(elapsedBlocks, BLOCK_CAPACITY - 1);
            auto& block = blocks_[blockIndex];
            block.weightedAngularVelocityX += static_cast<double>(sample.angularVelocity.x) * deltaSeconds;
            block.weightedAngularVelocityY += static_cast<double>(sample.angularVelocity.y) * deltaSeconds;
            block.weightedAngularVelocityZ += static_cast<double>(sample.angularVelocity.z) * deltaSeconds;
            block.durationSeconds += deltaSeconds;
            blockCount_ = std::max(blockCount_, blockIndex + 1);
        }
    }
    lastCollectedTimestampNanos_ = sample.timestampNanos;

    ++sampleCount_;
    double inverseCount = 1.0 / static_cast<double>(sampleCount_);
    double deltaX = static_cast<double>(sample.angularVelocity.x) - meanX_;
    double deltaY = static_cast<double>(sample.angularVelocity.y) - meanY_;
    double deltaZ = static_cast<double>(sample.angularVelocity.z) - meanZ_;
    meanX_ += deltaX * inverseCount;
    meanY_ += deltaY * inverseCount;
    meanZ_ += deltaZ * inverseCount;
    sumSquaredDifferencesX_ += deltaX * (static_cast<double>(sample.angularVelocity.x) - meanX_);
    sumSquaredDifferencesY_ += deltaY * (static_cast<double>(sample.angularVelocity.y) - meanY_);
    sumSquaredDifferencesZ_ += deltaZ * (static_cast<double>(sample.angularVelocity.z) - meanZ_);

    if (!angularVelocityExtentsInitialized_) {
        minimumAngularVelocity_ = sample.angularVelocity;
        maximumAngularVelocity_ = sample.angularVelocity;
        angularVelocityExtentsInitialized_ = true;
    } else {
        minimumAngularVelocity_.x = std::min(minimumAngularVelocity_.x, sample.angularVelocity.x);
        minimumAngularVelocity_.y = std::min(minimumAngularVelocity_.y, sample.angularVelocity.y);
        minimumAngularVelocity_.z = std::min(minimumAngularVelocity_.z, sample.angularVelocity.z);
        maximumAngularVelocity_.x = std::max(maximumAngularVelocity_.x, sample.angularVelocity.x);
        maximumAngularVelocity_.y = std::max(maximumAngularVelocity_.y, sample.angularVelocity.y);
        maximumAngularVelocity_.z = std::max(maximumAngularVelocity_.z, sample.angularVelocity.z);
    }
}

void GyroCalibration::accumulateAccelerometer(AccelerometerSample const& sample) {
    int64_t previousTimestampNanos = previousAccelerometerTimestampNanos_;
    if (accelerometerSampleCount_ == 0) {
        firstAccelerometerTimestampNanos_ = sample.timestampNanos;
    } else {
        int64_t deltaNanos = sample.timestampNanos - previousAccelerometerTimestampNanos_;
        if (deltaNanos > 0 && deltaNanos <= MAXIMUM_SAMPLE_GAP_NANOS) {
            double deltaSeconds = static_cast<double>(deltaNanos) * NANOSECONDS_TO_SECONDS;
            addCompensated(static_cast<double>(sample.acceleration.x) * deltaSeconds, weightedAccelerationX_,
                           weightedAccelerationCorrectionX_);
            addCompensated(static_cast<double>(sample.acceleration.y) * deltaSeconds, weightedAccelerationY_,
                           weightedAccelerationCorrectionY_);
            addCompensated(static_cast<double>(sample.acceleration.z) * deltaSeconds, weightedAccelerationZ_,
                           weightedAccelerationCorrectionZ_);
            weightedAccelerationDurationSeconds_ += deltaSeconds;

            int64_t intervalMidpointNanos = previousTimestampNanos + deltaNanos / 2;
            int64_t elapsedNanos = std::max(int64_t { 0 }, intervalMidpointNanos - collectionStartNanos_);
            std::size_t elapsedBlocks = static_cast<std::size_t>(elapsedNanos / BLOCK_DURATION_NANOS);
            std::size_t blockIndex = std::min(elapsedBlocks, BLOCK_CAPACITY - 1);
            auto& block = accelerometerBlocks_[blockIndex];
            block.weightedAccelerationX += static_cast<double>(sample.acceleration.x) * deltaSeconds;
            block.weightedAccelerationY += static_cast<double>(sample.acceleration.y) * deltaSeconds;
            block.weightedAccelerationZ += static_cast<double>(sample.acceleration.z) * deltaSeconds;
            block.durationSeconds += deltaSeconds;
            accelerometerBlockCount_ = std::max(accelerometerBlockCount_, blockIndex + 1);
        }
    }
    previousAccelerometerTimestampNanos_ = lastAccelerometerTimestampNanos_ = sample.timestampNanos;

    ++accelerometerSampleCount_;
    double inverseCount = 1.0 / static_cast<double>(accelerometerSampleCount_);
    double deltaX = static_cast<double>(sample.acceleration.x) - meanAccelerationX_;
    double deltaY = static_cast<double>(sample.acceleration.y) - meanAccelerationY_;
    double deltaZ = static_cast<double>(sample.acceleration.z) - meanAccelerationZ_;
    meanAccelerationX_ += deltaX * inverseCount;
    meanAccelerationY_ += deltaY * inverseCount;
    meanAccelerationZ_ += deltaZ * inverseCount;
    sumSquaredAccelerationDifferencesX_ += deltaX * (static_cast<double>(sample.acceleration.x) - meanAccelerationX_);
    sumSquaredAccelerationDifferencesY_ += deltaY * (static_cast<double>(sample.acceleration.y) - meanAccelerationY_);
    sumSquaredAccelerationDifferencesZ_ += deltaZ * (static_cast<double>(sample.acceleration.z) - meanAccelerationZ_);

    float magnitudeError = std::abs(sample.acceleration.magnitude() - EARTH_GRAVITY_METERS_PER_SECOND_SQUARED);
    peakAccelerationMagnitudeError_ = std::max(peakAccelerationMagnitudeError_, magnitudeError);
}

GyroCalibration::Result GyroCalibration::finish() const {
    Result result;
    if (weightedDurationSeconds_ > 0.0) {
        result.bias = { static_cast<float>(weightedAngularVelocityX_ / weightedDurationSeconds_),
                        static_cast<float>(weightedAngularVelocityY_ / weightedDurationSeconds_),
                        static_cast<float>(weightedAngularVelocityZ_ / weightedDurationSeconds_) };
    } else {
        result.bias = { static_cast<float>(meanX_), static_cast<float>(meanY_), static_cast<float>(meanZ_) };
    }

    // A raw angular-rate peak includes the zero-rate bias that calibration is trying to
    // measure. Test the extrema relative to that candidate bias instead.
    if (angularVelocityExtentsInitialized_) {
        Vec3 minimumDeviation = minimumAngularVelocity_ - result.bias;
        Vec3 maximumDeviation = maximumAngularVelocity_ - result.bias;
        result.peakAngularDeviation = maximumDeviation;
        if (std::abs(minimumDeviation.x) > std::abs(maximumDeviation.x))
            result.peakAngularDeviation.x = minimumDeviation.x;
        if (std::abs(minimumDeviation.y) > std::abs(maximumDeviation.y))
            result.peakAngularDeviation.y = minimumDeviation.y;
        if (std::abs(minimumDeviation.z) > std::abs(maximumDeviation.z))
            result.peakAngularDeviation.z = minimumDeviation.z;
        result.peakAngularDeviationMagnitude = result.peakAngularDeviation.magnitude();
    }

    result.standardDeviationLimit = LatiteMath::deg2rad(0.5f);
    result.peakAngularDeviationLimit = LatiteMath::deg2rad(5.f);
    result.blockMeanDeltaLimit = LatiteMath::deg2rad(0.05f);
    result.measurementDurationSeconds = weightedDurationSeconds_;
    result.sampleCount = sampleCount_;
    result.receivedGyroSampleCount = receivedGyroSampleCount_;
    result.rejectedGyroSampleCount = rejectedGyroSampleCount_;
    std::size_t roundedSampleCount = receivedGyroSampleCount_ + REJECTED_GYRO_SAMPLE_LIMIT_DIVISOR - 1;
    std::size_t proportionalLimit = roundedSampleCount / REJECTED_GYRO_SAMPLE_LIMIT_DIVISOR;
    result.rejectedGyroSampleLimit = std::max(MINIMUM_REJECTED_GYRO_SAMPLE_LIMIT, proportionalLimit);
    result.accelerometerAssisted = accelerometerAssisted_;
    result.accelerometerRmsDeviationLimit = 0.1f;
    result.meanGravityErrorLimit = 0.5f;
    result.peakAccelerationMagnitudeErrorLimit = 0.5f;
    result.accelerationBlockMeanDeltaLimit = 0.1f;
    result.accelerometerMeasurementDurationSeconds = weightedAccelerationDurationSeconds_;
    result.accelerometerSampleCount = accelerometerSampleCount_;

    if (weightedAccelerationDurationSeconds_ > 0.0) {
        result.meanAcceleration = { static_cast<float>(weightedAccelerationX_ / weightedAccelerationDurationSeconds_),
                                    static_cast<float>(weightedAccelerationY_ / weightedAccelerationDurationSeconds_),
                                    static_cast<float>(weightedAccelerationZ_ / weightedAccelerationDurationSeconds_) };
    } else if (accelerometerSampleCount_ > 0) {
        result.meanAcceleration = { static_cast<float>(meanAccelerationX_), static_cast<float>(meanAccelerationY_),
                                    static_cast<float>(meanAccelerationZ_) };
    }
    result.meanGravityError = std::abs(result.meanAcceleration.magnitude() - EARTH_GRAVITY_METERS_PER_SECOND_SQUARED);
    result.peakAccelerationMagnitudeError = peakAccelerationMagnitudeError_;

    if (sampleCount_ > 1) {
        double divisor = static_cast<double>(sampleCount_ - 1);
        result.standardDeviation = { static_cast<float>(std::sqrt(std::max(0.0, sumSquaredDifferencesX_ / divisor))),
                                     static_cast<float>(std::sqrt(std::max(0.0, sumSquaredDifferencesY_ / divisor))),
                                     static_cast<float>(std::sqrt(std::max(0.0, sumSquaredDifferencesZ_ / divisor))) };
    }
    if (accelerometerSampleCount_ > 1) {
        double divisor = static_cast<double>(accelerometerSampleCount_ - 1);
        double varianceX = sumSquaredAccelerationDifferencesX_ / divisor;
        double varianceY = sumSquaredAccelerationDifferencesY_ / divisor;
        double varianceZ = sumSquaredAccelerationDifferencesZ_ / divisor;
        double vectorVariance = varianceX + varianceY + varianceZ;
        result.accelerometerRmsDeviation = static_cast<float>(std::sqrt(std::max(0.0, vectorVariance)));
    }

    int64_t durationNanos = lastCollectedTimestampNanos_ - firstCollectedTimestampNanos_;
    if (durationNanos > 0 && sampleCount_ > 1)
        result.sampleRate = static_cast<float>(sampleCount_ - 1) /
                            static_cast<float>(static_cast<double>(durationNanos) * NANOSECONDS_TO_SECONDS);
    int64_t accelDurationNanos = lastAccelerometerTimestampNanos_ - firstAccelerometerTimestampNanos_;
    if (accelDurationNanos > 0 && accelerometerSampleCount_ > 1)
        result.accelerometerSampleRate =
            static_cast<float>(accelerometerSampleCount_ - 1) /
            static_cast<float>(static_cast<double>(accelDurationNanos) * NANOSECONDS_TO_SECONDS);

    double minimumDuration = static_cast<double>(collectionDurationNanos_) * NANOSECONDS_TO_SECONDS * 0.8;
    if (result.rejectedGyroSampleCount > result.rejectedGyroSampleLimit) {
        result.failureReason = FailureReason::MotionDetected;
        return result;
    }
    if (sampleCount_ < minimumSampleCount_ || weightedDurationSeconds_ < minimumDuration) {
        result.failureReason = FailureReason::InsufficientSamples;
        return result;
    }
    if (accelerometerAssisted_ && (accelerometerSampleCount_ < minimumAccelerometerSampleCount_ ||
                                   weightedAccelerationDurationSeconds_ < minimumDuration)) {
        result.failureReason = FailureReason::InsufficientAccelerometerSamples;
        return result;
    }

    bool hasPreviousBlock = false;
    double previousBlockMeanX = 0.0, previousBlockMeanY = 0.0, previousBlockMeanZ = 0.0;
    for (std::size_t index = 0; index < blockCount_; ++index) {
        auto const& block = blocks_[index];
        if (block.durationSeconds < MINIMUM_BLOCK_DURATION_SECONDS) continue;
        double meanX = block.weightedAngularVelocityX / block.durationSeconds;
        double meanY = block.weightedAngularVelocityY / block.durationSeconds;
        double meanZ = block.weightedAngularVelocityZ / block.durationSeconds;
        if (hasPreviousBlock) {
            result.maximumBlockMeanDelta.x =
                std::max(result.maximumBlockMeanDelta.x, static_cast<float>(std::abs(meanX - previousBlockMeanX)));
            result.maximumBlockMeanDelta.y =
                std::max(result.maximumBlockMeanDelta.y, static_cast<float>(std::abs(meanY - previousBlockMeanY)));
            result.maximumBlockMeanDelta.z =
                std::max(result.maximumBlockMeanDelta.z, static_cast<float>(std::abs(meanZ - previousBlockMeanZ)));
        }
        previousBlockMeanX = meanX;
        previousBlockMeanY = meanY;
        previousBlockMeanZ = meanZ;
        hasPreviousBlock = true;
    }

    bool hasPreviousAccelerationBlock = false;
    Vec3 previousAccelerationBlockMean;
    for (std::size_t index = 0; index < accelerometerBlockCount_; ++index) {
        auto const& block = accelerometerBlocks_[index];
        if (block.durationSeconds < MINIMUM_BLOCK_DURATION_SECONDS) continue;
        float meanX = static_cast<float>(block.weightedAccelerationX / block.durationSeconds);
        float meanY = static_cast<float>(block.weightedAccelerationY / block.durationSeconds);
        float meanZ = static_cast<float>(block.weightedAccelerationZ / block.durationSeconds);
        Vec3 blockMean { meanX, meanY, meanZ };
        if (hasPreviousAccelerationBlock)
            result.maximumAccelerationBlockMeanDelta = std::max(
                result.maximumAccelerationBlockMeanDelta, (blockMean - previousAccelerationBlockMean).magnitude());
        previousAccelerationBlockMean = blockMean;
        hasPreviousAccelerationBlock = true;
    }

    float gyroNoise = std::max({ result.standardDeviation.x, result.standardDeviation.y, result.standardDeviation.z });
    bool gyroNoiseFailed = !std::isfinite(gyroNoise) || gyroNoise > result.standardDeviationLimit;
    float gyroPeakDeviation = result.peakAngularDeviationMagnitude;
    bool gyroPeakFailed = !std::isfinite(gyroPeakDeviation) || gyroPeakDeviation > result.peakAngularDeviationLimit;

    float accelNoise = result.accelerometerRmsDeviation;
    float accelNoiseLimit = result.accelerometerRmsDeviationLimit;
    bool accelNoiseFailed = !std::isfinite(accelNoise) || accelNoise > accelNoiseLimit;
    float gravityError = result.meanGravityError;
    bool gravityFailed = !std::isfinite(gravityError) || gravityError > result.meanGravityErrorLimit;
    float accelPeak = result.peakAccelerationMagnitudeError;
    float accelPeakLimit = result.peakAccelerationMagnitudeErrorLimit;
    bool accelPeakFailed = !std::isfinite(accelPeak) || accelPeak > accelPeakLimit;
    float accelDrift = result.maximumAccelerationBlockMeanDelta;
    float accelDriftLimit = result.accelerationBlockMeanDeltaLimit;
    bool accelDriftFailed = !std::isfinite(accelDrift) || accelDrift > accelDriftLimit;
    bool accelerationFailed = accelNoiseFailed || gravityFailed;
    accelerationFailed |= accelPeakFailed || accelDriftFailed;
    accelerationFailed &= accelerometerAssisted_;

    float gyroDrift = std::max(result.maximumBlockMeanDelta.x, result.maximumBlockMeanDelta.y);
    gyroDrift = std::max(gyroDrift, result.maximumBlockMeanDelta.z);
    bool gyroDriftFailed = !std::isfinite(gyroDrift) || gyroDrift > result.blockMeanDeltaLimit;

    if (gyroNoiseFailed || gyroPeakFailed || accelerationFailed)
        result.failureReason = FailureReason::MotionDetected;
    else if (gyroDriftFailed)
        result.failureReason = FailureReason::BiasUnstable;
    return result;
}
