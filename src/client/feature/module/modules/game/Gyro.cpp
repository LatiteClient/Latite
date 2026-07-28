#include "pch.h"
#include "Gyro.h"

#include "client/event/events/FocusLostEvent.h"
#include "client/event/events/KeyUpdateEvent.h"
#include "client/event/events/TurnDeltaEvent.h"
#include "client/event/events/UpdateEvent.h"
#include "client/misc/Notifications.h"
#include "client/screen/ScreenManager.h"
#include "client/screen/screens/GyroCalibrationScreen.h"

#include <algorithm>
#include <cmath>
#include <span>

namespace {
    constexpr auto MAX_PENDING_SAMPLE_AGE = std::chrono::milliseconds(250);
    constexpr auto CALIBRATION_TIMEOUT_GRACE = std::chrono::milliseconds(2500);
    constexpr float MAXIMUM_VERIFICATION_BIAS_DELTA = LatiteMath::deg2rad(0.05f);
    constexpr float DEGREES_PER_RADIAN = 180.f / pi_f;

    Vec3 radiansToDegrees(Vec3 const& value) {
        return value * DEGREES_PER_RADIAN;
    }

    float maximumAbsoluteComponent(Vec3 const& value) {
        return std::max({ std::abs(value.x), std::abs(value.y), std::abs(value.z) });
    }

    Vec3 weightedBias(GyroCalibration::Result const& initial, GyroCalibration::Result const& verification) {
        double totalDuration = initial.measurementDurationSeconds + verification.measurementDurationSeconds;
        if (totalDuration <= 0.0) return verification.bias;
        return initial.bias * static_cast<float>(initial.measurementDurationSeconds / totalDuration) +
               verification.bias * static_cast<float>(verification.measurementDurationSeconds / totalDuration);
    }

    char const* calibrationFailureKey(GyroCalibration::FailureReason reason, bool verification) {
        if (verification) {
            switch (reason) {
            case GyroCalibration::FailureReason::InsufficientSamples:
                return "client.module.gyro.calibration.verificationFailedSamples";
            case GyroCalibration::FailureReason::InsufficientAccelerometerSamples:
                return "client.module.gyro.calibration.verificationFailedAccelerometerSamples";
            case GyroCalibration::FailureReason::BiasUnstable:
                return "client.module.gyro.calibration.verificationFailedStability";
            case GyroCalibration::FailureReason::MotionDetected:
            case GyroCalibration::FailureReason::None:
                return "client.module.gyro.calibration.verificationFailedMotion";
            }
        }

        switch (reason) {
        case GyroCalibration::FailureReason::InsufficientSamples:
            return "client.module.gyro.calibration.failedSamples";
        case GyroCalibration::FailureReason::InsufficientAccelerometerSamples:
            return "client.module.gyro.calibration.failedAccelerometerSamples";
        case GyroCalibration::FailureReason::BiasUnstable:
            return "client.module.gyro.calibration.failedStability";
        case GyroCalibration::FailureReason::MotionDetected:
        case GyroCalibration::FailureReason::None:
            return "client.module.gyro.calibration.failedMotion";
        }
        return "client.module.gyro.calibration.failedMotion";
    }
}

Gyro::Gyro()
    : Module("Gyro", LocalizeString::get("client.module.gyro.name"), LocalizeString::get("client.module.gyro.desc"),
             GAME, nokeybind) {
    inputSourceData.addEntry({ 0, LocalizeString::get("client.module.gyro.inputSource.auto.name"),
                               LocalizeString::get("client.module.gyro.inputSource.auto.desc") });
    inputSourceData.addEntry({ 1, LocalizeString::get("client.module.gyro.inputSource.system.name"),
                               LocalizeString::get("client.module.gyro.inputSource.system.desc") });
    inputSourceData.addEntry({ 2, LocalizeString::get("client.module.gyro.inputSource.controller.name"),
                               LocalizeString::get("client.module.gyro.inputSource.controller.desc") });

    deviceData.addEntry({ 0, LocalizeString::get("client.module.gyro.device.auto.name"),
                          LocalizeString::get("client.module.gyro.device.auto.desc") });
    deviceSelectionIds.emplace_back();

    activationModeData.addEntry({ 0, LocalizeString::get("client.module.gyro.activationMode.toggle.name"),
                                  LocalizeString::get("client.module.gyro.activationMode.toggle.desc") });
    activationModeData.addEntry({ 1, LocalizeString::get("client.module.gyro.activationMode.holdEnable.name"),
                                  LocalizeString::get("client.module.gyro.activationMode.holdEnable.desc") });
    activationModeData.addEntry({ 2, LocalizeString::get("client.module.gyro.activationMode.holdPause.name"),
                                  LocalizeString::get("client.module.gyro.activationMode.holdPause.desc") });

    std::shared_ptr<Setting> sourceSetting;
    sourceSetting = addEnumSetting("inputSource", LocalizeString::get("client.module.gyro.inputSource.name"),
                                   LocalizeString::get("client.module.gyro.inputSource.desc"), inputSourceData);
    sourceSetting->callback = [this](Setting&) {
        sensor.setRequestedSource(requestedSource());
        if (isEnabled()) refreshDeviceSetting();
        handleSourceChange(true);
    };
    std::shared_ptr<Setting> deviceSetting;
    deviceSetting = addEnumSetting("device", LocalizeString::get("client.module.gyro.device.name"),
                                   LocalizeString::get("client.module.gyro.device.desc"), deviceData);
    deviceSetting->callback = [this](Setting&) {
        int selectedIndex = deviceData.getSelectedKey();
        pendingDeviceIndex = selectedIndex;
        selectedDeviceId.clear();
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(deviceSelectionIds.size())) {
            selectedDeviceId = deviceSelectionIds[selectedIndex];
            pendingDeviceIndex = -1;
        }
        sensor.setRequestedDevice(selectedDeviceId);
        handleSourceChange(true);
    };
    addSetting("activationKey", LocalizeString::get("client.module.gyro.activationKey.name"),
               LocalizeString::get("client.module.gyro.activationKey.desc"), activationKey);
    addSliderSetting("sensitivity", LocalizeString::get("client.module.gyro.sensitivity.name"),
                     LocalizeString::get("client.module.gyro.sensitivity.desc"), sensitivity, FloatValue(0.f),
                     FloatValue(40.f), FloatValue(0.05f));
    addSliderSetting("verticalRatio", LocalizeString::get("client.module.gyro.verticalRatio.name"),
                     LocalizeString::get("client.module.gyro.verticalRatio.desc"), verticalRatio, FloatValue(0.1f),
                     FloatValue(2.f), FloatValue(0.05f));
    addSetting("dynamicSensitivity", LocalizeString::get("client.module.gyro.dynamicSensitivity.name"),
               LocalizeString::get("client.module.gyro.dynamicSensitivity.desc"), dynamicSensitivity);
    addSliderSetting("fastSensitivity", LocalizeString::get("client.module.gyro.fastSensitivity.name"),
                     LocalizeString::get("client.module.gyro.fastSensitivity.desc"), fastSensitivity, FloatValue(0.f),
                     FloatValue(20.f), FloatValue(0.05f), "dynamicSensitivity"_istrue);
    std::shared_ptr<Setting> accelerationStart;
    accelerationStart = addSliderSetting(
        "accelerationStartSpeed", LocalizeString::get("client.module.gyro.accelerationStartSpeed.name"),
        LocalizeString::get("client.module.gyro.accelerationStartSpeed.desc"), accelerationStartSpeed, FloatValue(0.f),
        FloatValue(500.f), FloatValue(1.f), "dynamicSensitivity"_istrue);
    accelerationStart->callback = [this](Setting&) {
        auto& start = std::get<FloatValue>(accelerationStartSpeed).value;
        auto& full = std::get<FloatValue>(accelerationFullSpeed).value;
        if (start >= full) full = std::min(start + 1.f, 1000.f);
    };
    std::shared_ptr<Setting> accelerationFull;
    accelerationFull =
        addSliderSetting("accelerationFullSpeed", LocalizeString::get("client.module.gyro.accelerationFullSpeed.name"),
                         LocalizeString::get("client.module.gyro.accelerationFullSpeed.desc"), accelerationFullSpeed,
                         FloatValue(1.f), FloatValue(1000.f), FloatValue(1.f), "dynamicSensitivity"_istrue);
    accelerationFull->callback = [this](Setting&) {
        auto& start = std::get<FloatValue>(accelerationStartSpeed).value;
        auto& full = std::get<FloatValue>(accelerationFullSpeed).value;
        if (full <= start) start = std::max(full - 1.f, 0.f);
    };
    addSliderSetting("deadzoneCutoff", LocalizeString::get("client.module.gyro.deadzoneCutoff.name"),
                     LocalizeString::get("client.module.gyro.deadzoneCutoff.desc"), deadzoneCutoff, FloatValue(0.f),
                     FloatValue(10.f), FloatValue(0.05f));
    addSliderSetting("deadzoneRecoveryWidth", LocalizeString::get("client.module.gyro.deadzoneRecoveryWidth.name"),
                     LocalizeString::get("client.module.gyro.deadzoneRecoveryWidth.desc"), deadzoneRecoveryWidth,
                     FloatValue(0.f), FloatValue(20.f), FloatValue(0.05f));
    addSliderSetting("smoothingTime", LocalizeString::get("client.module.gyro.smoothingTime.name"),
                     LocalizeString::get("client.module.gyro.smoothingTime.desc"), smoothingTime, FloatValue(0.f),
                     FloatValue(125.f), FloatValue(1.f));
    addSliderSetting("smoothingBypassSpeed", LocalizeString::get("client.module.gyro.smoothingBypassSpeed.name"),
                     LocalizeString::get("client.module.gyro.smoothingBypassSpeed.desc"), smoothingBypassSpeed,
                     FloatValue(0.f), FloatValue(100.f), FloatValue(1.f));
    std::shared_ptr<Setting> activationSetting;
    activationSetting =
        addEnumSetting("activationMode", LocalizeString::get("client.module.gyro.activationMode.name"),
                       LocalizeString::get("client.module.gyro.activationMode.desc"), activationModeData);
    activationSetting->callback = [this](Setting&) {
        if (isEnabled()) resetActivationState();
    };
    addActionSetting("calibrate", LocalizeString::get("client.module.gyro.calibrate.name"),
                     LocalizeString::get("client.module.gyro.calibrate.desc"), [this] {
                         showCalibrationScreen(CalibrationPurpose::ApplyBias);
                     });
    addActionSetting("diagnostics", LocalizeString::get("client.module.gyro.diagnostics.name"),
                     LocalizeString::get("client.module.gyro.diagnostics.desc"), [this] {
                         showCalibrationScreen(CalibrationPurpose::MeasureNoise);
                     });
    addSetting("invertHorizontal", LocalizeString::get("client.module.gyro.invertHorizontal.name"),
               LocalizeString::get("client.module.gyro.invertHorizontal.desc"), invertHorizontal);
    addSetting("invertVertical", LocalizeString::get("client.module.gyro.invertVertical.name"),
               LocalizeString::get("client.module.gyro.invertVertical.desc"), invertVertical);

    listen<TurnDeltaEvent>(static_cast<EventListenerFunc>(&Gyro::onTurnDelta));
    listen<KeyUpdateEvent>(static_cast<EventListenerFunc>(&Gyro::onKeyUpdate));
    listen<FocusLostEvent>(static_cast<EventListenerFunc>(&Gyro::onFocusLost));
    listen<UpdateEvent>(static_cast<EventListenerFunc>(&Gyro::onUpdate));
}

Gyro::~Gyro() {
    sensor.stop();
}

void Gyro::onEnable() {
    resetActivationState();
    processor.setCalibrationBias({});
    sensor.setRequestedDevice(selectedDeviceId);
    WindowsGyroscope::StartResult startResult;
    startResult = sensor.start(
        requestedSource(),
        [this](Vec3 const& value, int64_t timestamp) {
            addSample(value, timestamp);
        },
        [this](Vec3 const& value, int64_t timestamp) {
            addAccelerometerSample(value, timestamp);
        });
    observedDeviceGeneration = sensor.deviceGeneration();
    refreshDeviceSetting();
    observedSourceGeneration = sensor.sourceGeneration();

    if (!startResult.inputAvailable) {
        Latite::getNotifications().push(LocalizeString::get("client.module.gyro.inputUnavailable"));
        return;
    }
    if (!startResult.sensorAvailable)
        Latite::getNotifications().push(unavailableMessage());
    else
        Latite::getNotifications().push(LocalizeString::get("client.module.gyro.connected").value() +
                                        sensor.activeDeviceName());
}

void Gyro::onDisable() {
    gyroActive = false;
    cancelCalibration();
    resetInput();
    sensor.stop();
}

void Gyro::onTurnDelta(Event& event) {
    handleSourceChange(true);
    if (calibrationActive.load(std::memory_order_acquire)) return;

    auto* client = SDK::ClientInstance::get();
    if (!gyroActive || !client || !client->minecraftGame || !client->getLocalPlayer() ||
        !client->minecraftGame->isCursorGrabbed() || Latite::get().getScreenManager().getActiveScreen()) {
        resetInput();
        return;
    }

    beginAcceptingSamples();
    auto& turnEvent = reinterpret_cast<TurnDeltaEvent&>(event);
    turnEvent.setDelta(turnEvent.getDelta() + consumeCameraDelta());
}

void Gyro::onKeyUpdate(Event& event) {
    auto& keyEvent = reinterpret_cast<KeyUpdateEvent&>(event);
    if (keyEvent.getKey() != std::get<KeyValue>(activationKey).value) return;

    bool pressed = keyEvent.isDown() && !activationKeyDown;
    activationKeyDown = keyEvent.isDown();
    ActivationMode mode = static_cast<ActivationMode>(activationModeData.getSelectedKey());
    if (keyEvent.inUI()) {
        if (mode == ActivationMode::HoldToEnable) setGyroActive(false);
        if (mode == ActivationMode::HoldToPause) setGyroActive(true);
        return;
    }

    switch (mode) {
    case ActivationMode::Toggle:
        if (pressed) setGyroActive(!gyroActive);
        break;
    case ActivationMode::HoldToEnable:
        setGyroActive(keyEvent.isDown());
        break;
    case ActivationMode::HoldToPause:
        setGyroActive(!keyEvent.isDown());
        break;
    }
}

void Gyro::onFocusLost(Event&) {
    bool calibrationWasRunning = calibrationDeadline != std::chrono::steady_clock::time_point {};
    CalibrationPurpose interruptedPurpose = calibrationPurpose;
    cancelCalibration();
    if (calibrationWasRunning) {
        showCalibrationResult(interruptedPurpose, false,
                              LocalizeString::get("client.screen.gyroCalibration.focusLost"));
    }
    activationKeyDown = false;
    if (static_cast<ActivationMode>(activationModeData.getSelectedKey()) == ActivationMode::Toggle)
        resetInput();
    else
        resetActivationState();
}

void Gyro::onUpdate(Event&) {
    uint64_t deviceGeneration = sensor.deviceGeneration();
    if (deviceGeneration != observedDeviceGeneration) {
        observedDeviceGeneration = deviceGeneration;
        refreshDeviceSetting();
    }
    handleSourceChange(true);

    std::optional<CompletedCalibration> completed;
    {
        std::scoped_lock lock { sampleMutex };
        completed = std::exchange(completedCalibration, std::nullopt);
    }
    if (completed) {
        finishCalibration(*completed);
        return;
    }

    if (!calibrationActive.load(std::memory_order_acquire)) return;

    auto& screen = Latite::getScreenManager().get<GyroCalibrationScreen>();
    screen.updateSampleCount(calibrationSamplesReceived.load(std::memory_order_acquire));
    if (calibrationDeadline == std::chrono::steady_clock::time_point {} ||
        std::chrono::steady_clock::now() < calibrationDeadline) {
        return;
    }

    CalibrationPurpose timedOutPurpose = calibrationPurpose;
    cancelCalibration();
    if (timedOutPurpose == CalibrationPurpose::MeasureNoise) {
        showCalibrationResult(timedOutPurpose, false,
                              LocalizeString::get("client.module.gyro.calibration.diagnosticsFailed"),
                              LocalizeString::get("client.screen.gyroCalibration.noSamples"));
        return;
    }

    bool verification = timedOutPurpose == CalibrationPurpose::VerifyBias;
    showCalibrationResult(
        timedOutPurpose, false,
        LocalizeString::get(calibrationFailureKey(GyroCalibration::FailureReason::InsufficientSamples, verification)));
}

void Gyro::addSample(Vec3 const& angularVelocity, int64_t timestampNanos) {
    GyroSample sample { angularVelocity, timestampNanos };
    if (calibrationActive.load(std::memory_order_acquire)) {
        std::scoped_lock lock { sampleMutex };
        if (!calibrationActive.load(std::memory_order_relaxed) ||
            timestampNanos <= activationCutoffNanos.load(std::memory_order_relaxed))
            return;
        calibrationSamplesReceived.fetch_add(1, std::memory_order_acq_rel);
        auto result = calibration.addSample(sample);
        if (result) {
            completedCalibration = CompletedCalibration { *result, calibrationPurpose };
            calibrationActive.store(false, std::memory_order_release);
        }
        return;
    }

    if (!acceptingSamples.load(std::memory_order_acquire) ||
        timestampNanos <= activationCutoffNanos.load(std::memory_order_acquire))
        return;

    auto now = std::chrono::steady_clock::now();
    std::scoped_lock lock { sampleMutex };
    if (!acceptingSamples.load(std::memory_order_relaxed) ||
        timestampNanos <= activationCutoffNanos.load(std::memory_order_relaxed))
        return;
    if (now - lastConsume > MAX_PENDING_SAMPLE_AGE) {
        sampleQueueStart = sampleQueueSize = 0;
        sampleDiscontinuity = true;
        return;
    }
    if (sampleQueueSize == SAMPLE_QUEUE_CAPACITY) {
        sampleQueueStart = (sampleQueueStart + 1) % SAMPLE_QUEUE_CAPACITY;
        --sampleQueueSize;
        sampleDiscontinuity = true;
    }
    std::size_t index = (sampleQueueStart + sampleQueueSize) % SAMPLE_QUEUE_CAPACITY;
    sampleQueue[index] = sample;
    ++sampleQueueSize;
}

void Gyro::addAccelerometerSample(Vec3 const& acceleration, int64_t timestampNanos) {
    if (!calibrationActive.load(std::memory_order_acquire) ||
        !calibrationAccelerometerAvailable.load(std::memory_order_acquire) ||
        timestampNanos <= activationCutoffNanos.load(std::memory_order_acquire))
        return;
    std::scoped_lock lock { sampleMutex };
    if (!calibrationActive.load(std::memory_order_relaxed) ||
        timestampNanos <= activationCutoffNanos.load(std::memory_order_relaxed))
        return;
    calibration.addAccelerometerSample({ acceleration, timestampNanos });
}

void Gyro::beginAcceptingSamples() {
    if (inputActive) return;
    acceptingSamples.store(false, std::memory_order_release);
    activationCutoffNanos.store(sensor.currentTimestampNanos(), std::memory_order_release);
    {
        std::scoped_lock lock { sampleMutex };
        sampleQueueStart = sampleQueueSize = 0;
        sampleDiscontinuity = false;
        lastConsume = std::chrono::steady_clock::now();
    }
    processor.reset();
    inputActive = true;
    acceptingSamples.store(true, std::memory_order_release);
}

void Gyro::resetInput() {
    if (!inputActive && !acceptingSamples.load(std::memory_order_relaxed)) return;
    acceptingSamples.store(false, std::memory_order_release);
    activationCutoffNanos.store(sensor.currentTimestampNanos(), std::memory_order_release);
    {
        std::scoped_lock lock { sampleMutex };
        sampleQueueStart = sampleQueueSize = 0;
        sampleDiscontinuity = false;
        lastConsume = std::chrono::steady_clock::now();
    }
    processor.reset();
    inputActive = false;
}

Gyro::SampleBatch Gyro::drainSamples() {
    std::scoped_lock lock { sampleMutex };
    SampleBatch batch;
    batch.size = sampleQueueSize;
    batch.discontinuity = sampleDiscontinuity;
    for (std::size_t index = 0; index < sampleQueueSize; ++index)
        batch.samples[index] = sampleQueue[(sampleQueueStart + index) % SAMPLE_QUEUE_CAPACITY];
    sampleQueueStart = sampleQueueSize = 0;
    sampleDiscontinuity = false;
    lastConsume = std::chrono::steady_clock::now();
    return batch;
}

Vec2 Gyro::consumeCameraDelta() {
    SampleBatch batch = drainSamples();
    if (batch.discontinuity) processor.reset();
    GyroProcessor::Settings currentSettings {
        .sensitivity = std::get<FloatValue>(sensitivity).value,
        .verticalRatio = std::get<FloatValue>(verticalRatio).value,
        .fastSensitivity = std::get<FloatValue>(fastSensitivity).value,
        .accelerationStartSpeed = std::get<FloatValue>(accelerationStartSpeed).value,
        .accelerationFullSpeed = std::get<FloatValue>(accelerationFullSpeed).value,
        .deadzoneCutoff = std::get<FloatValue>(deadzoneCutoff).value,
        .deadzoneRecoveryWidth = std::get<FloatValue>(deadzoneRecoveryWidth).value,
        .smoothingTime = std::get<FloatValue>(smoothingTime).value,
        .smoothingBypassSpeed = std::get<FloatValue>(smoothingBypassSpeed).value,
        .dynamicSensitivity = std::get<BoolValue>(dynamicSensitivity).value,
        .invertHorizontal = std::get<BoolValue>(invertHorizontal).value,
        .invertVertical = std::get<BoolValue>(invertVertical).value
    };
    return processor.process(std::span<GyroSample const> { batch.samples.data(), batch.size }, currentSettings);
}

void Gyro::setGyroActive(bool active) {
    if (gyroActive == active) return;
    gyroActive = active;
    resetInput();
}

void Gyro::resetActivationState() {
    activationKeyDown = false;
    gyroActive = static_cast<ActivationMode>(activationModeData.getSelectedKey()) != ActivationMode::HoldToEnable;
    resetInput();
}

void Gyro::handleSourceChange(bool announce) {
    uint64_t generation = sensor.sourceGeneration();
    if (generation == observedSourceGeneration) return;
    observedSourceGeneration = generation;

    bool calibrationWasRunning = calibrationDeadline != std::chrono::steady_clock::time_point {};
    CalibrationPurpose interruptedPurpose = calibrationPurpose;
    cancelCalibration();
    resetInput();
    processor.setCalibrationBias({});

    if (calibrationWasRunning) {
        showCalibrationResult(interruptedPurpose, false,
                              LocalizeString::get("client.screen.gyroCalibration.sourceChanged"));
    }
    if (!announce) return;
    if (sensor.activeSource() == WindowsGyroscope::ActiveSource::None)
        Latite::getNotifications().push(unavailableMessage());
    else
        Latite::getNotifications().push(LocalizeString::get("client.module.gyro.connected").value() +
                                        sensor.activeDeviceName());
}

void Gyro::refreshDeviceSetting() {
    std::vector<WindowsGyroscope::DeviceInfo> available = sensor.availableDevices();
    WindowsGyroscope::RequestedSource source = requestedSource();
    std::erase_if(available, [source](WindowsGyroscope::DeviceInfo const& device) {
        if (source == WindowsGyroscope::RequestedSource::Controller)
            return device.source != WindowsGyroscope::ActiveSource::Controller;
        if (source == WindowsGyroscope::RequestedSource::SystemSensor)
            return device.source != WindowsGyroscope::ActiveSource::SystemSensor;
        return false;
    });

    std::vector<EnumEntry> entries;
    std::vector<std::string> ids;
    entries.emplace_back(0, LocalizeString::get("client.module.gyro.device.auto.name"),
                         LocalizeString::get("client.module.gyro.device.auto.desc"));
    ids.emplace_back();

    std::vector<std::wstring> labels;
    labels.reserve(available.size());
    for (auto const& device : available) {
        std::wstring backendName;
        if (device.source == WindowsGyroscope::ActiveSource::Controller)
            backendName = LocalizeString::get("client.module.gyro.device.backend.sdl");
        else
            backendName = LocalizeString::get("client.module.gyro.device.backend.gameInput");
        labels.push_back(device.name + L" [" + backendName + L"]");
    }

    for (std::size_t index = 0; index < available.size(); ++index) {
        auto const& device = available[index];
        std::wstring label = labels[index];
        int matchingLabels = static_cast<int>(std::count(labels.begin(), labels.end(), label));
        if (matchingLabels > 1) {
            int occurrence = static_cast<int>(std::count(labels.begin(), labels.begin() + index, label)) + 1;
            label += L" (" + std::to_wstring(occurrence) + L")";
        }

        char const* descriptionKey = "client.module.gyro.device.system.desc";
        if (device.source == WindowsGyroscope::ActiveSource::Controller)
            descriptionKey = "client.module.gyro.device.sdl.desc";

        ids.push_back(device.id);
        entries.emplace_back(static_cast<int>(index + 1), label, LocalizeString::get(descriptionKey));
    }

    std::string desiredId = selectedDeviceId;
    if (desiredId.empty() && pendingDeviceIndex > 0 && pendingDeviceIndex < static_cast<int>(ids.size()))
        desiredId = ids[pendingDeviceIndex];

    int selectedIndex = 0;
    auto selected = std::find(ids.begin(), ids.end(), desiredId);
    if (selected != ids.end()) selectedIndex = static_cast<int>(std::distance(ids.begin(), selected));

    selectedDeviceId = ids[selectedIndex];
    pendingDeviceIndex = -1;
    deviceSelectionIds = std::move(ids);
    deviceData.replaceEntries(std::move(entries), selectedIndex);
    sensor.setRequestedDevice(selectedDeviceId);
}

WindowsGyroscope::RequestedSource Gyro::requestedSource() const {
    switch (static_cast<InputSource>(inputSourceData.getSelectedKey())) {
    case InputSource::SystemSensor:
        return WindowsGyroscope::RequestedSource::SystemSensor;
    case InputSource::Controller:
        return WindowsGyroscope::RequestedSource::Controller;
    case InputSource::Auto:
        return WindowsGyroscope::RequestedSource::Auto;
    }
    return WindowsGyroscope::RequestedSource::Auto;
}

std::wstring Gyro::unavailableMessage() const {
    switch (static_cast<InputSource>(inputSourceData.getSelectedKey())) {
    case InputSource::SystemSensor:
        return LocalizeString::get("client.module.gyro.systemUnavailable");
    case InputSource::Controller:
        return LocalizeString::get("client.module.gyro.controllerUnavailable");
    case InputSource::Auto:
        return LocalizeString::get("client.module.gyro.unavailable");
    }
    return LocalizeString::get("client.module.gyro.unavailable");
}

void Gyro::showCalibrationScreen(CalibrationPurpose purpose) {
    auto& screenManager = Latite::getScreenManager();
    auto& screen = screenManager.get<GyroCalibrationScreen>();
    GyroCalibrationScreen::Purpose screenPurpose = GyroCalibrationScreen::Purpose::Calibration;
    if (purpose == CalibrationPurpose::MeasureNoise) screenPurpose = GyroCalibrationScreen::Purpose::Diagnostics;
    std::wstring deviceName = sensor.activeDeviceName();

    if (!isEnabled()) {
        screen.showResult(screenPurpose, false, std::move(deviceName),
                          LocalizeString::get("client.module.gyro.calibration.requiresEnabled"));
        screenManager.showScreen<GyroCalibrationScreen>();
        return;
    }
    if (sensor.activeSource() == WindowsGyroscope::ActiveSource::None) {
        screen.showResult(screenPurpose, false, std::move(deviceName), unavailableMessage());
        screenManager.showScreen<GyroCalibrationScreen>();
        return;
    }

    screen.showPrompt(screenPurpose, std::move(deviceName), [this, purpose] {
        startCalibration(purpose);
    });
    screenManager.showScreen<GyroCalibrationScreen>();
}

void Gyro::startCalibration(CalibrationPurpose purpose) {
    if (!isEnabled()) {
        showCalibrationResult(purpose, false, LocalizeString::get("client.module.gyro.calibration.requiresEnabled"));
        return;
    }
    if (sensor.activeSource() == WindowsGyroscope::ActiveSource::None) {
        showCalibrationResult(purpose, false, unavailableMessage());
        return;
    }

    if (purpose != CalibrationPurpose::VerifyBias) {
        cancelCalibration();
        initialCalibrationResult.reset();
    }
    beginCalibration(purpose);
}

void Gyro::beginCalibration(CalibrationPurpose purpose) {
    calibrationActive.store(false, std::memory_order_release);
    resetInput();
    activationCutoffNanos.store(sensor.currentTimestampNanos(), std::memory_order_release);

    GyroCalibrationScreen::Purpose screenPurpose = GyroCalibrationScreen::Purpose::Calibration;
    GyroCalibrationScreen::ProgressStage progressStage = GyroCalibrationScreen::ProgressStage::Measuring;
    std::chrono::milliseconds expectedDuration;
    {
        std::scoped_lock lock { sampleMutex };
        GyroCalibration::Mode mode = GyroCalibration::Mode::Diagnostics;
        if (purpose == CalibrationPurpose::ApplyBias) {
            mode = GyroCalibration::Mode::Calibration;
        } else if (purpose == CalibrationPurpose::VerifyBias) {
            mode = GyroCalibration::Mode::Verification;
            progressStage = GyroCalibrationScreen::ProgressStage::Verifying;
        } else {
            screenPurpose = GyroCalibrationScreen::Purpose::Diagnostics;
            progressStage = GyroCalibrationScreen::ProgressStage::Diagnosing;
        }
        bool hasAccelerometer = sensor.activeDeviceHasAccelerometer();
        calibrationAccelerometerAvailable.store(hasAccelerometer, std::memory_order_release);
        expectedDuration = calibration.begin(mode, hasAccelerometer);
        calibrationPurpose = purpose;
        completedCalibration.reset();
    }

    calibrationSamplesReceived.store(0, std::memory_order_release);
    calibrationDeadline = std::chrono::steady_clock::now() + expectedDuration + CALIBRATION_TIMEOUT_GRACE;
    calibrationActive.store(true, std::memory_order_release);

    auto& screenManager = Latite::getScreenManager();
    auto& screen = screenManager.get<GyroCalibrationScreen>();
    screen.showProgress(screenPurpose, progressStage, sensor.activeDeviceName(), expectedDuration, [this] {
        cancelCalibration();
    });
    screenManager.showScreen<GyroCalibrationScreen>();
}

void Gyro::cancelCalibration() {
    calibrationActive.store(false, std::memory_order_release);
    calibrationDeadline = {};
    calibrationSamplesReceived.store(0, std::memory_order_release);
    std::scoped_lock lock { sampleMutex };
    calibration.cancel();
    completedCalibration.reset();
    initialCalibrationResult.reset();
    calibrationAccelerometerAvailable.store(false, std::memory_order_release);
}

void Gyro::finishCalibration(CompletedCalibration const& completed) {
    calibrationDeadline = {};
    calibrationSamplesReceived.store(0, std::memory_order_release);
    calibrationAccelerometerAvailable.store(false, std::memory_order_release);

    auto const& result = completed.result;
    if (completed.purpose == CalibrationPurpose::ApplyBias) {
        if (!result.successful()) {
            showCalibrationResult(completed.purpose, false,
                                  LocalizeString::get(calibrationFailureKey(result.failureReason, false)));
            return;
        }
        initialCalibrationResult = result;
        beginCalibration(CalibrationPurpose::VerifyBias);
        return;
    }

    if (completed.purpose == CalibrationPurpose::VerifyBias) {
        if (!initialCalibrationResult) {
            showCalibrationResult(completed.purpose, false,
                                  LocalizeString::get("client.screen.gyroCalibration.unexpectedFailure"));
            return;
        }
        GyroCalibration::Result initial = *initialCalibrationResult;
        initialCalibrationResult.reset();
        if (!result.successful()) {
            showCalibrationResult(completed.purpose, false,
                                  LocalizeString::get(calibrationFailureKey(result.failureReason, true)));
            return;
        }

        Vec3 biasDelta = result.bias - initial.bias;
        if (maximumAbsoluteComponent(biasDelta) > MAXIMUM_VERIFICATION_BIAS_DELTA) {
            Vec3 deltaDegrees = radiansToDegrees(biasDelta);
            showCalibrationResult(
                completed.purpose, false, LocalizeString::get("client.module.gyro.calibration.verificationFailedBias"),
                std::format(L"Verification drift difference (X / Y / Z): {:.4f} / {:.4f} / {:.4f} deg/s "
                            L"(limit: {:.4f} deg/s)",
                            deltaDegrees.x, deltaDegrees.y, deltaDegrees.z,
                            MAXIMUM_VERIFICATION_BIAS_DELTA * DEGREES_PER_RADIAN));
            return;
        }

        Vec3 appliedBias = weightedBias(initial, result);
        processor.setCalibrationBias(appliedBias);
        Vec3 appliedDegrees = radiansToDegrees(appliedBias);
        Vec3 initialNoise = radiansToDegrees(initial.standardDeviation);
        Vec3 verificationNoise = radiansToDegrees(result.standardDeviation);
        float maximumNoise = maximumAbsoluteComponent(initialNoise);
        maximumNoise = std::max(maximumNoise, maximumAbsoluteComponent(verificationNoise));
        bool accelerometerAssisted = initial.accelerometerAssisted && result.accelerometerAssisted;
        std::size_t acceptedSamples = initial.sampleCount + result.sampleCount;
        std::size_t receivedSamples = initial.receivedGyroSampleCount + result.receivedGyroSampleCount;
        std::size_t rejectedSamples = initial.rejectedGyroSampleCount + result.rejectedGyroSampleCount;
        showCalibrationResult(completed.purpose, true, LocalizeString::get("client.module.gyro.calibration.success"),
                              std::format(L"Applied correction (X / Y / Z): {:.4f} / {:.4f} / {:.4f} deg/s\n"
                                          L"Motion check: {}\nStationary noise: {:.3f} deg/s\n"
                                          L"Samples: {} accepted / {} received\nRejected transients: {}",
                                          appliedDegrees.x, appliedDegrees.y, appliedDegrees.z,
                                          accelerometerAssisted ? L"gyro + accelerometer" : L"gyro only", maximumNoise,
                                          acceptedSamples, receivedSamples, rejectedSamples));
        return;
    }

    Vec3 biasDegrees = radiansToDegrees(result.bias);
    Vec3 noiseDegrees = radiansToDegrees(result.standardDeviation);
    Vec3 peakDeviationDegrees = radiansToDegrees(result.peakAngularDeviation);
    Vec3 blockDeltaDegrees = radiansToDegrees(result.maximumBlockMeanDelta);
    std::string summaryKey = "client.module.gyro.calibration.diagnosticsFailed";
    if (result.successful()) summaryKey = "client.module.gyro.calibration.diagnosticsSuccess";
    std::wstring summary = LocalizeString::get(summaryKey);
    std::wstring accelerationDetail;
    if (result.accelerometerAssisted) {
        accelerationDetail = std::format(
            L"\nMean acceleration (X / Y / Z): {:.3f} / {:.3f} / {:.3f} m/s^2\n"
            L"Acceleration RMS deviation: {:.3f} m/s^2 (limit: {:.3f})\n"
            L"Mean gravity error: {:.3f} m/s^2 (limit: {:.3f})\n"
            L"Peak acceleration magnitude error: {:.3f} m/s^2 (limit: {:.3f})\n"
            L"Maximum adjacent-block acceleration change: {:.3f} m/s^2 (limit: {:.3f})\n"
            L"Accelerometer sample rate: {:.1f} Hz\nAccelerometer samples: {}",
            result.meanAcceleration.x, result.meanAcceleration.y, result.meanAcceleration.z,
            result.accelerometerRmsDeviation, result.accelerometerRmsDeviationLimit, result.meanGravityError,
            result.meanGravityErrorLimit, result.peakAccelerationMagnitudeError,
            result.peakAccelerationMagnitudeErrorLimit, result.maximumAccelerationBlockMeanDelta,
            result.accelerationBlockMeanDeltaLimit, result.accelerometerSampleRate, result.accelerometerSampleCount);
    }
    showCalibrationResult(
        completed.purpose, result.successful(), summary,
        std::format(L"Mean rate (X / Y / Z): {:.4f} / {:.4f} / {:.4f} deg/s\n"
                    L"Motion check: {}\n"
                    L"Standard deviation (X / Y / Z): {:.4f} / {:.4f} / {:.4f} deg/s (limit: {:.3f})\n"
                    L"Peak deviation (X / Y / Z): {:.3f} / {:.3f} / {:.3f} deg/s\n"
                    L"Peak deviation bound: {:.3f} deg/s (limit: {:.3f})\n"
                    L"Maximum adjacent-block mean change (X / Y / Z): {:.3f} / {:.3f} / {:.3f} deg/s "
                    L"(limit: {:.3f})\nSample rate: {:.1f} Hz\n"
                    L"Samples: {} accepted / {} received\nRejected transients: {} (limit: {}){}",
                    biasDegrees.x, biasDegrees.y, biasDegrees.z,
                    result.accelerometerAssisted ? L"gyro + accelerometer" : L"gyro only", noiseDegrees.x,
                    noiseDegrees.y, noiseDegrees.z, result.standardDeviationLimit * DEGREES_PER_RADIAN,
                    peakDeviationDegrees.x, peakDeviationDegrees.y, peakDeviationDegrees.z,
                    result.peakAngularDeviationMagnitude * DEGREES_PER_RADIAN,
                    result.peakAngularDeviationLimit * DEGREES_PER_RADIAN, blockDeltaDegrees.x, blockDeltaDegrees.y,
                    blockDeltaDegrees.z, result.blockMeanDeltaLimit * DEGREES_PER_RADIAN, result.sampleRate,
                    result.sampleCount, result.receivedGyroSampleCount, result.rejectedGyroSampleCount,
                    result.rejectedGyroSampleLimit, accelerationDetail));
}

void Gyro::showCalibrationResult(CalibrationPurpose purpose, bool successful, std::wstring const& summary,
                                 std::wstring const& detail) {
    GyroCalibrationScreen::Purpose screenPurpose = GyroCalibrationScreen::Purpose::Calibration;
    if (purpose == CalibrationPurpose::MeasureNoise) screenPurpose = GyroCalibrationScreen::Purpose::Diagnostics;

    auto& screenManager = Latite::getScreenManager();
    auto& screen = screenManager.get<GyroCalibrationScreen>();
    screen.showResult(screenPurpose, successful, sensor.activeDeviceName(), summary, detail);
    screenManager.showScreen<GyroCalibrationScreen>();
}
