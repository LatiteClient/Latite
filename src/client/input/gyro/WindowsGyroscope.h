#pragma once

#include <GameInput.h>
#include <wrl/client.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include "client/input/ControllerInput.h"
#include "util/LMath.h"

class WindowsGyroscope final {
public:
    enum class RequestedSource {
        Auto,
        SystemSensor,
        Controller
    };

    enum class ActiveSource {
        None,
        SystemSensor,
        Controller
    };

    struct DeviceInfo {
        std::string id;
        std::wstring name;
        ActiveSource source;
    };

    struct StartResult {
        bool inputAvailable = false;
        bool sensorAvailable = false;
    };

    using GyroCallback = std::function<void(Vec3 const&, int64_t)>;
    using AccelerometerCallback = std::function<void(Vec3 const&, int64_t)>;

    WindowsGyroscope() = default;
    ~WindowsGyroscope();

    WindowsGyroscope(WindowsGyroscope const&) = delete;
    WindowsGyroscope& operator=(WindowsGyroscope const&) = delete;

    StartResult start(RequestedSource source, GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback);
    void stop();
    void setRequestedSource(RequestedSource source);
    void setRequestedDevice(std::string deviceId);

    [[nodiscard]] ActiveSource activeSource() const;
    [[nodiscard]] std::string activeDeviceId() const;
    [[nodiscard]] std::wstring activeDeviceName() const;
    [[nodiscard]] bool activeDeviceHasAccelerometer() const;
    [[nodiscard]] std::vector<DeviceInfo> availableDevices() const;
    [[nodiscard]] uint64_t sourceGeneration() const;
    [[nodiscard]] uint64_t deviceGeneration() const;
    [[nodiscard]] int64_t currentTimestampNanos() const;

private:
    using IGameInput = GameInput::v3::IGameInput;
    using IGameInputDevice = GameInput::v3::IGameInputDevice;
    using IGameInputReading = GameInput::v3::IGameInputReading;

    struct SystemSensorEntry {
        Microsoft::WRL::ComPtr<IGameInputDevice> device;
        std::string id;
        std::wstring name;
        bool hasAccelerometer = false;
    };

    static void CALLBACK systemSensorReadingCallback(GameInput::v3::GameInputCallbackToken token, void* context,
                                                     IGameInputReading* reading);
    static void CALLBACK systemSensorDeviceCallback(GameInput::v3::GameInputCallbackToken token, void* context,
                                                    IGameInputDevice* device, uint64_t timestamp,
                                                    GameInput::v3::GameInputDeviceStatus currentStatus,
                                                    GameInput::v3::GameInputDeviceStatus previousStatus);

    HRESULT startSystemSensorInput();
    void stopSystemSensorInput();
    void handleSystemSensorReading(IGameInputReading* reading);
    void handleSystemSensorDevice(IGameInputDevice* device, GameInput::v3::GameInputDeviceStatus currentStatus);
    void handleSdlGyro(std::string const& deviceId, Vec3 const& angularVelocity, int64_t timestampNanos);
    void handleSdlAccelerometer(std::string const& deviceId, Vec3 const& acceleration, int64_t timestampNanos);
    void handleSdlDevices(std::vector<ControllerInput::SensorDeviceState> const& states);
    void selectActiveDeviceLocked();

    mutable std::mutex mutex;
    Microsoft::WRL::ComPtr<IGameInput> systemSensorInput;
    std::vector<SystemSensorEntry> systemSensors;
    Microsoft::WRL::ComPtr<IGameInputDevice> activeSystemSensor;
    std::vector<ControllerInput::SensorDeviceState> sdlDevices;
    std::string requestedDeviceId;
    std::string selectedDeviceId;
    std::wstring activeName;
    RequestedSource requestedSource = RequestedSource::Auto;
    ActiveSource selectedSource = ActiveSource::None;
    bool selectedHasAccelerometer = false;
    GyroCallback onGyro;
    AccelerometerCallback onAccelerometer;
    GameInput::v3::GameInputCallbackToken systemSensorReadingToken = 0;
    GameInput::v3::GameInputCallbackToken systemSensorDeviceToken = 0;
    std::atomic_bool running = false;
    std::atomic<uint64_t> generation = 0;
    std::atomic<uint64_t> devicesGeneration = 0;
};
