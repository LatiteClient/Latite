#pragma once

#include "../../Module.h"
#include "client/input/gyro/GyroCalibration.h"
#include "client/input/gyro/GyroProcessor.h"
#include "client/input/gyro/WindowsGyroscope.h"

#include <array>
#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

class Gyro final : public Module {
public:
    Gyro();
    ~Gyro() override;

    void onEnable() override;
    void onDisable() override;

private:
    static constexpr std::size_t SAMPLE_QUEUE_CAPACITY = 64;

    enum class InputSource {
        Auto,
        SystemSensor,
        Controller
    };

    enum class ActivationMode {
        Toggle,
        HoldToEnable,
        HoldToPause
    };

    enum class CalibrationPurpose {
        ApplyBias,
        VerifyBias,
        MeasureNoise
    };

    struct SampleBatch {
        std::array<GyroSample, SAMPLE_QUEUE_CAPACITY> samples;
        std::size_t size = 0;
        bool discontinuity = false;
    };

    struct CompletedCalibration {
        GyroCalibration::Result result;
        CalibrationPurpose purpose;
    };

    void onTurnDelta(Event& event);
    void onKeyUpdate(Event& event);
    void onFocusLost(Event& event);
    void onUpdate(Event& event);

    void addSample(Vec3 const& angularVelocity, int64_t timestampNanos);
    void addAccelerometerSample(Vec3 const& acceleration, int64_t timestampNanos);
    void beginAcceptingSamples();
    void resetInput();
    SampleBatch drainSamples();
    Vec2 consumeCameraDelta();
    float consumeFlickStickDelta();
    void resetFlickStick();
    [[nodiscard]] bool isFlickStickBlocked() const;

    void setGyroActive(bool active);
    void resetActivationState();
    void handleSourceChange(bool announce);
    void refreshDeviceSetting();
    [[nodiscard]] WindowsGyroscope::RequestedSource requestedSource() const;
    [[nodiscard]] std::wstring unavailableMessage() const;

    void showCalibrationScreen(CalibrationPurpose purpose);
    void startCalibration(CalibrationPurpose purpose);
    void beginCalibration(CalibrationPurpose purpose);
    void cancelCalibration();
    void finishCalibration(CompletedCalibration const& completed);
    void showCalibrationResult(CalibrationPurpose purpose, bool successful, std::wstring const& summary,
                               std::wstring const& detail = L"");

    std::mutex sampleMutex;
    std::array<GyroSample, SAMPLE_QUEUE_CAPACITY> sampleQueue;
    std::size_t sampleQueueStart = 0;
    std::size_t sampleQueueSize = 0;
    bool sampleDiscontinuity = false;
    std::chrono::steady_clock::time_point lastConsume = std::chrono::steady_clock::now();
    std::atomic_bool acceptingSamples = false;
    std::atomic<int64_t> activationCutoffNanos = 0;
    bool inputActive = false;

    WindowsGyroscope sensor;
    uint64_t observedSourceGeneration = 0;
    uint64_t observedDeviceGeneration = 0;
    GyroProcessor processor;
    GyroCalibration calibration;
    std::atomic_bool calibrationActive = false;
    CalibrationPurpose calibrationPurpose = CalibrationPurpose::ApplyBias;
    std::optional<CompletedCalibration> completedCalibration;
    std::optional<GyroCalibration::Result> initialCalibrationResult;
    std::atomic_bool calibrationAccelerometerAvailable = false;
    std::atomic<std::size_t> calibrationSamplesReceived = 0;
    std::chrono::steady_clock::time_point calibrationDeadline = {};

    EnumData inputSourceData;
    EnumData deviceData;
    std::vector<std::string> deviceSelectionIds;
    std::string selectedDeviceId;
    int pendingDeviceIndex = -1;
    EnumData activationModeData;
    ValueType activationKey = KeyValue('G');
    ValueType sensitivity = FloatValue(1.f);
    ValueType verticalRatio = FloatValue(1.f);
    ValueType dynamicSensitivity = BoolValue(false);
    ValueType fastSensitivity = FloatValue(1.f);
    ValueType accelerationStartSpeed = FloatValue(20.f);
    ValueType accelerationFullSpeed = FloatValue(120.f);
    ValueType deadzoneCutoff = FloatValue(0.f);
    ValueType deadzoneRecoveryWidth = FloatValue(0.f);
    ValueType smoothingTime = FloatValue(0.f);
    ValueType smoothingBypassSpeed = FloatValue(0.f);
    ValueType invertHorizontal = BoolValue(false);
    ValueType invertVertical = BoolValue(false);
    ValueType flickStick = BoolValue(false);
    ValueType flickStickDeadzone = FloatValue(0.8f);
    ValueType flickDuration = FloatValue(80.f);
    ValueType sweepSensitivity = FloatValue(1.f);
    ValueType invertFlickStick = BoolValue(false);
    bool gyroActive = false;
    bool activationKeyDown = false;
    bool flickStickBlocked = false;
    bool flickStickEngaged = false;
    float previousFlickStickAngle = 0.f;
    float flickAnimationTarget = 0.f;
    float flickAnimationApplied = 0.f;
    std::chrono::steady_clock::time_point flickAnimationStart = {};
};
