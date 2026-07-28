#include "pch.h"
#include "WindowsGyroscope.h"

#include "client/Latite.h"

#include <algorithm>
#include <cmath>
#include <utility>

using namespace GameInput::v3;
using Microsoft::WRL::ComPtr;

namespace {
    constexpr float METERS_PER_SECOND_SQUARED_PER_G = 9.80665f;
    constexpr int64_t NANOSECONDS_PER_GAMEINPUT_MICROSECOND = 1'000;

    std::string systemSensorId(GameInputDeviceInfo const& info, IGameInputDevice* device) {
        if (info.pnpPath && *info.pnpPath != '\0') return "gameinput:path:" + std::string(info.pnpPath);

        std::string name = info.displayName && *info.displayName != '\0' ? info.displayName : "motion-sensor";
        return "gameinput:session:" + name + ":" + std::to_string(reinterpret_cast<uintptr_t>(device));
    }
}

WindowsGyroscope::~WindowsGyroscope() {
    stop();
}

WindowsGyroscope::StartResult WindowsGyroscope::start(RequestedSource source, GyroCallback gyroCallback,
                                                      AccelerometerCallback accelerometerCallback) {
    stop();

    {
        std::scoped_lock lock { mutex };
        requestedSource = source;
        onGyro = std::move(gyroCallback);
        onAccelerometer = std::move(accelerometerCallback);
        running.store(true, std::memory_order_release);
    }

    bool sdlAvailable;
    sdlAvailable = Latite::get().getControllerInput().startSensors(
        [this](std::string const& id, Vec3 const& value, int64_t timestamp) {
            handleSdlGyro(id, value, timestamp);
        },
        [this](std::string const& id, Vec3 const& value, int64_t timestamp) {
            handleSdlAccelerometer(id, value, timestamp);
        },
        [this](std::vector<ControllerInput::SensorDeviceState> const& states) {
            handleSdlDevices(states);
        });
    HRESULT systemSensorResult = startSystemSensorInput();

    bool sensorAvailable = false;
    {
        std::scoped_lock lock { mutex };
        selectActiveDeviceLocked();
        sensorAvailable = selectedSource != ActiveSource::None;
    }

    bool inputAvailable = sdlAvailable || SUCCEEDED(systemSensorResult);
    return { inputAvailable, sensorAvailable };
}

HRESULT WindowsGyroscope::startSystemSensorInput() {
    ComPtr<IGameInput> input;
    HRESULT result = GameInputCreate(input.GetAddressOf());
    if (FAILED(result)) return result;

    {
        std::scoped_lock lock { mutex };
        systemSensorInput = input;
    }

    GameInputCallbackToken newDeviceToken = 0;
    result =
        input->RegisterDeviceCallback(nullptr, GameInputKindSensors, GameInputDeviceAnyStatus,
                                      GameInputBlockingEnumeration, this, systemSensorDeviceCallback, &newDeviceToken);
    if (FAILED(result)) {
        std::scoped_lock lock { mutex };
        systemSensorInput.Reset();
        return result;
    }

    GameInputCallbackToken newReadingToken = 0;
    result = input->RegisterReadingCallback(nullptr, GameInputKindSensors, this, systemSensorReadingCallback,
                                            &newReadingToken);
    if (FAILED(result)) {
        input->UnregisterCallback(newDeviceToken);
        std::scoped_lock lock { mutex };
        systemSensors.clear();
        activeSystemSensor.Reset();
        systemSensorInput.Reset();
        return result;
    }

    {
        std::scoped_lock lock { mutex };
        systemSensorDeviceToken = newDeviceToken;
        systemSensorReadingToken = newReadingToken;
    }
    return S_OK;
}

void WindowsGyroscope::stop() {
    bool hadDevices = false;
    {
        std::scoped_lock lock { mutex };
        running.store(false, std::memory_order_release);
        hadDevices = !sdlDevices.empty() || !systemSensors.empty();
    }

    Latite::get().getControllerInput().stopSensors();
    stopSystemSensorInput();

    {
        std::scoped_lock lock { mutex };
        bool hadSource = selectedSource != ActiveSource::None;
        sdlDevices.clear();
        selectedDeviceId.clear();
        activeName.clear();
        selectedSource = ActiveSource::None;
        selectedHasAccelerometer = false;
        onGyro = {};
        onAccelerometer = {};
        if (hadSource) generation.fetch_add(1, std::memory_order_acq_rel);
        if (hadDevices) devicesGeneration.fetch_add(1, std::memory_order_acq_rel);
    }
}

void WindowsGyroscope::stopSystemSensorInput() {
    ComPtr<IGameInput> input;
    GameInputCallbackToken oldReadingToken = 0;
    GameInputCallbackToken oldDeviceToken = 0;

    {
        std::scoped_lock lock { mutex };
        input = systemSensorInput;
        oldReadingToken = std::exchange(systemSensorReadingToken, 0);
        oldDeviceToken = std::exchange(systemSensorDeviceToken, 0);
    }

    // Unregistration waits for callbacks, which also take mutex.
    if (input && oldReadingToken != 0) input->UnregisterCallback(oldReadingToken);
    if (input && oldDeviceToken != 0) input->UnregisterCallback(oldDeviceToken);

    std::scoped_lock lock { mutex };
    systemSensors.clear();
    activeSystemSensor.Reset();
    systemSensorInput.Reset();
}

void WindowsGyroscope::setRequestedSource(RequestedSource source) {
    std::scoped_lock lock { mutex };
    if (requestedSource == source) return;
    requestedSource = source;
    selectActiveDeviceLocked();
}

void WindowsGyroscope::setRequestedDevice(std::string deviceId) {
    std::scoped_lock lock { mutex };
    if (requestedDeviceId == deviceId) return;
    requestedDeviceId = std::move(deviceId);
    selectActiveDeviceLocked();
}

WindowsGyroscope::ActiveSource WindowsGyroscope::activeSource() const {
    std::scoped_lock lock { mutex };
    return selectedSource;
}

std::wstring WindowsGyroscope::activeDeviceName() const {
    std::scoped_lock lock { mutex };
    return activeName;
}

bool WindowsGyroscope::activeDeviceHasAccelerometer() const {
    std::scoped_lock lock { mutex };
    return selectedHasAccelerometer;
}

std::vector<WindowsGyroscope::DeviceInfo> WindowsGyroscope::availableDevices() const {
    std::scoped_lock lock { mutex };

    std::vector<DeviceInfo> result;
    result.reserve(sdlDevices.size() + systemSensors.size());
    for (auto const& device : sdlDevices) {
        result.push_back({ device.id, device.name, ActiveSource::Controller });
    }
    for (auto const& sensor : systemSensors) {
        result.push_back({ sensor.id, sensor.name, ActiveSource::SystemSensor });
    }
    return result;
}

uint64_t WindowsGyroscope::sourceGeneration() const {
    return generation.load(std::memory_order_acquire);
}

uint64_t WindowsGyroscope::deviceGeneration() const {
    return devicesGeneration.load(std::memory_order_acquire);
}

int64_t WindowsGyroscope::currentTimestampNanos() const {
    ComPtr<IGameInput> input;
    ActiveSource source = ActiveSource::None;
    std::string id;
    {
        std::scoped_lock lock { mutex };
        source = selectedSource;
        id = selectedDeviceId;
        input = systemSensorInput;
    }

    if (source == ActiveSource::Controller) return Latite::get().getControllerInput().currentSensorTimestampNanos(id);
    if (source != ActiveSource::SystemSensor || !input) return 0;
    return static_cast<int64_t>(input->GetCurrentTimestamp()) * NANOSECONDS_PER_GAMEINPUT_MICROSECOND;
}

void CALLBACK WindowsGyroscope::systemSensorReadingCallback(GameInputCallbackToken, void* context,
                                                            IGameInputReading* reading) {
    static_cast<WindowsGyroscope*>(context)->handleSystemSensorReading(reading);
}

void CALLBACK WindowsGyroscope::systemSensorDeviceCallback(GameInputCallbackToken, void* context,
                                                           IGameInputDevice* device, uint64_t,
                                                           GameInputDeviceStatus currentStatus, GameInputDeviceStatus) {
    static_cast<WindowsGyroscope*>(context)->handleSystemSensorDevice(device, currentStatus);
}

void WindowsGyroscope::handleSystemSensorReading(IGameInputReading* reading) {
    if (!reading || !running.load(std::memory_order_acquire)) return;

    ComPtr<IGameInputDevice> readingDevice;
    reading->GetDevice(readingDevice.GetAddressOf());

    GyroCallback gyroHandler;
    AccelerometerCallback accelerometerHandler;
    bool hasAccelerometer = false;
    {
        std::scoped_lock lock { mutex };
        if (!running.load(std::memory_order_relaxed) || selectedSource != ActiveSource::SystemSensor ||
            !activeSystemSensor || readingDevice.Get() != activeSystemSensor.Get()) {
            return;
        }
        gyroHandler = onGyro;
        accelerometerHandler = onAccelerometer;
        hasAccelerometer = selectedHasAccelerometer;
    }

    GameInputSensorsState state {};
    if (!reading->GetSensorsState(&state)) return;

    int64_t timestampNanos = static_cast<int64_t>(reading->GetTimestamp()) * NANOSECONDS_PER_GAMEINPUT_MICROSECOND;
    // GameInput reports clockwise yaw as negative Y. Bedrock consumes positive yaw for a right turn.
    Vec3 angularVelocity { state.angularVelocityInRadPerSecX, -state.angularVelocityInRadPerSecY,
                           state.angularVelocityInRadPerSecZ };
    if (gyroHandler && std::isfinite(angularVelocity.x) && std::isfinite(angularVelocity.y) &&
        std::isfinite(angularVelocity.z)) {
        gyroHandler(angularVelocity, timestampNanos);
    }

    Vec3 acceleration { state.accelerationInGX * METERS_PER_SECOND_SQUARED_PER_G,
                        state.accelerationInGY * METERS_PER_SECOND_SQUARED_PER_G,
                        state.accelerationInGZ * METERS_PER_SECOND_SQUARED_PER_G };
    if (hasAccelerometer && accelerometerHandler && std::isfinite(acceleration.x) && std::isfinite(acceleration.y) &&
        std::isfinite(acceleration.z)) {
        accelerometerHandler(acceleration, timestampNanos);
    }
}

void WindowsGyroscope::handleSdlGyro(std::string const& deviceId, Vec3 const& angularVelocity, int64_t timestampNanos) {
    GyroCallback gyroHandler;
    {
        std::scoped_lock lock { mutex };
        if (!running.load(std::memory_order_relaxed) || selectedSource != ActiveSource::Controller ||
            selectedDeviceId != deviceId) {
            return;
        }
        gyroHandler = onGyro;
    }
    if (gyroHandler) gyroHandler(angularVelocity, timestampNanos);
}

void WindowsGyroscope::handleSdlAccelerometer(std::string const& deviceId, Vec3 const& acceleration,
                                              int64_t timestampNanos) {
    AccelerometerCallback accelerometerHandler;
    {
        std::scoped_lock lock { mutex };
        if (!running.load(std::memory_order_relaxed) || selectedSource != ActiveSource::Controller ||
            selectedDeviceId != deviceId || !selectedHasAccelerometer) {
            return;
        }
        accelerometerHandler = onAccelerometer;
    }
    if (accelerometerHandler) accelerometerHandler(acceleration, timestampNanos);
}

void WindowsGyroscope::handleSdlDevices(std::vector<ControllerInput::SensorDeviceState> const& states) {
    std::scoped_lock lock { mutex };
    if (!running.load(std::memory_order_relaxed)) return;

    bool devicesChanged = states.size() != sdlDevices.size();
    if (!devicesChanged) {
        for (std::size_t index = 0; index < states.size(); ++index) {
            auto const& current = states[index];
            auto const& previous = sdlDevices[index];
            if (current.id != previous.id || current.name != previous.name ||
                current.hasAccelerometer != previous.hasAccelerometer) {
                devicesChanged = true;
                break;
            }
        }
    }

    sdlDevices = states;
    if (devicesChanged) devicesGeneration.fetch_add(1, std::memory_order_acq_rel);
    selectActiveDeviceLocked();
}

void WindowsGyroscope::handleSystemSensorDevice(IGameInputDevice* device, GameInputDeviceStatus currentStatus) {
    if (!device || !running.load(std::memory_order_acquire)) return;

    std::scoped_lock lock { mutex };
    if (!running.load(std::memory_order_relaxed)) return;

    auto existing = std::find_if(systemSensors.begin(), systemSensors.end(), [device](SystemSensorEntry const& entry) {
        return entry.device.Get() == device;
    });

    GameInputDeviceInfo const* info = nullptr;
    bool connected = (currentStatus & GameInputDeviceConnected) != GameInputDeviceNoStatus;
    bool validSystemSensor = connected && SUCCEEDED(device->GetDeviceInfo(&info)) && info && info->sensorsInfo;
    GameInputSensorsKind supportedSensors = GameInputSensorsNone;
    if (validSystemSensor) {
        // SDL owns every controller path, including SDL's GameInput bridge.
        supportedSensors = info->sensorsInfo->supportedSensors;
        GameInputKind controllerKinds = GameInputKindController | GameInputKindGamepad;
        bool controller = (info->supportedInput & controllerKinds) != GameInputKindUnknown;
        bool hasGyroscope = (supportedSensors & GameInputSensorsGyrometer) != GameInputSensorsNone;
        validSystemSensor = !controller && hasGyroscope;
    }

    if (!validSystemSensor) {
        if (existing != systemSensors.end()) {
            systemSensors.erase(existing);
            devicesGeneration.fetch_add(1, std::memory_order_acq_rel);
        }
        selectActiveDeviceLocked();
        return;
    }

    SystemSensorEntry entry;
    entry.device = device;
    entry.id = systemSensorId(*info, device);
    entry.name = L"Motion sensor";
    if (info->displayName && *info->displayName != '\0') entry.name = util::StrToWStr(info->displayName);
    entry.hasAccelerometer = (supportedSensors & GameInputSensorsAccelerometer) != GameInputSensorsNone;

    for (auto const& other : systemSensors) {
        if (other.device.Get() == device || other.id != entry.id) continue;
        entry.id += ":instance:" + std::to_string(reinterpret_cast<uintptr_t>(device));
        break;
    }

    bool devicesChanged = existing == systemSensors.end();
    if (existing == systemSensors.end()) {
        systemSensors.emplace_back(std::move(entry));
    } else {
        devicesChanged = existing->id != entry.id;
        devicesChanged |= existing->name != entry.name;
        devicesChanged |= existing->hasAccelerometer != entry.hasAccelerometer;
        *existing = std::move(entry);
    }

    if (devicesChanged) devicesGeneration.fetch_add(1, std::memory_order_acq_rel);
    selectActiveDeviceLocked();
}

void WindowsGyroscope::selectActiveDeviceLocked() {
    IGameInputDevice* previousSystemSensor = activeSystemSensor.Get();
    std::string previousId = selectedDeviceId;
    std::wstring previousName = activeName;
    ActiveSource previousSource = selectedSource;
    bool previousHasAccelerometer = selectedHasAccelerometer;

    ControllerInput::SensorDeviceState const* selectedSdl = nullptr;
    SystemSensorEntry const* selectedSystemSensor = nullptr;
    bool controllerAllowed = requestedSource != RequestedSource::SystemSensor;
    bool systemSensorAllowed = requestedSource != RequestedSource::Controller;

    if (!requestedDeviceId.empty() && controllerAllowed) {
        for (auto const& controller : sdlDevices) {
            if (controller.id != requestedDeviceId) continue;
            selectedSdl = &controller;
            break;
        }
    }

    if (!requestedDeviceId.empty() && !selectedSdl && systemSensorAllowed) {
        for (auto const& sensor : systemSensors) {
            if (sensor.id != requestedDeviceId) continue;
            selectedSystemSensor = &sensor;
            break;
        }
    }

    if (!selectedSdl && !selectedSystemSensor && controllerAllowed && !sdlDevices.empty())
        selectedSdl = &sdlDevices.front();
    if (!selectedSdl && !selectedSystemSensor && systemSensorAllowed && !systemSensors.empty())
        selectedSystemSensor = &systemSensors.front();

    if (selectedSdl) {
        activeSystemSensor.Reset();
        selectedDeviceId = selectedSdl->id;
        activeName = selectedSdl->name;
        selectedSource = ActiveSource::Controller;
        selectedHasAccelerometer = selectedSdl->hasAccelerometer;
    } else if (selectedSystemSensor) {
        activeSystemSensor = selectedSystemSensor->device;
        selectedDeviceId = selectedSystemSensor->id;
        activeName = selectedSystemSensor->name;
        selectedSource = ActiveSource::SystemSensor;
        selectedHasAccelerometer = selectedSystemSensor->hasAccelerometer;
    } else {
        activeSystemSensor.Reset();
        selectedDeviceId.clear();
        activeName.clear();
        selectedSource = ActiveSource::None;
        selectedHasAccelerometer = false;
    }

    bool selectionChanged = previousSystemSensor != activeSystemSensor.Get();
    selectionChanged |= previousId != selectedDeviceId;
    selectionChanged |= previousName != activeName;
    selectionChanged |= previousSource != selectedSource;
    selectionChanged |= previousHasAccelerometer != selectedHasAccelerometer;
    if (selectionChanged) generation.fetch_add(1, std::memory_order_acq_rel);
}
