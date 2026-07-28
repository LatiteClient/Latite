#include "pch.h"
#include "SdlGamepadGyroscope.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <utility>

namespace {
    std::string deviceId(SDL_Gamepad* gamepad, SDL_JoystickID instanceId) {
        char const* path = SDL_GetGamepadPath(gamepad);
        if (path && *path != '\0') return "sdl:path:" + std::string(path);

        char const* serial = SDL_GetGamepadSerial(gamepad);
        if (serial && *serial != '\0') return "sdl:serial:" + std::string(serial);

        char const* name = SDL_GetGamepadName(gamepad);
        std::string fallbackName = name && *name != '\0' ? name : "gyro-controller";
        return "sdl:session:" + fallbackName + ":" + std::to_string(instanceId);
    }
}

struct SdlGamepadGyroscope::Impl {
    struct DeviceEntry {
        SDL_Gamepad* gamepad = nullptr;
        SDL_JoystickID instanceId = 0;
        DeviceState state;
        int64_t lastTimestampNanos = 0;
        int64_t lastAccelerometerTimestampNanos = 0;
    };

    static constexpr auto DEVICE_SCAN_INTERVAL = std::chrono::milliseconds(250);
    static constexpr auto INPUT_UPDATE_INTERVAL = std::chrono::milliseconds(1);

    static bool SDLCALL watchEvent(void* context, SDL_Event* event) {
        if (context && event) static_cast<Impl*>(context)->handleEvent(*event);
        return true;
    }

    bool initializeSdl() {
        SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_GAMEINPUT, "1", SDL_HINT_OVERRIDE);
        SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_ENHANCED_REPORTS, "auto", SDL_HINT_OVERRIDE);
        SDL_SetHintWithPriority(SDL_HINT_JOYSTICK_HIDAPI_SWITCH_PLAYER_LED, "0", SDL_HINT_OVERRIDE);

        if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) return false;
        sdlInitialized = true;

        if (!SDL_AddEventWatch(watchEvent, this)) {
            SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
            sdlInitialized = false;
            return false;
        }
        return true;
    }

    void handleEvent(SDL_Event const& event) {
        if (!running.load(std::memory_order_acquire)) return;

        if (event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED ||
            event.type == SDL_EVENT_GAMEPAD_REMAPPED) {
            requestDeviceScan();
            return;
        }
        if (event.type != SDL_EVENT_GAMEPAD_SENSOR_UPDATE) return;

        std::string id;
        GyroCallback gyroHandler;
        AccelerometerCallback accelerometerHandler;
        int64_t timestampNanos = static_cast<int64_t>(event.gsensor.sensor_timestamp);
        if (timestampNanos <= 0) timestampNanos = static_cast<int64_t>(event.gsensor.timestamp);

        {
            std::scoped_lock lock { mutex };
            auto device = std::find_if(devices.begin(), devices.end(), [&event](DeviceEntry const& entry) {
                return entry.instanceId == event.gsensor.which;
            });
            if (device == devices.end()) return;

            id = device->state.id;
            gyroHandler = onGyro;
            accelerometerHandler = onAccelerometer;
            // SDL's GameInput bridge can return the same current reading on
            // consecutive updates. Forward each sensor timestamp only once.
            if (event.gsensor.sensor == SDL_SENSOR_GYRO) {
                if (timestampNanos <= device->lastTimestampNanos) return;
                device->lastTimestampNanos = timestampNanos;
            } else if (event.gsensor.sensor == SDL_SENSOR_ACCEL) {
                if (timestampNanos <= device->lastAccelerometerTimestampNanos) return;
                device->lastAccelerometerTimestampNanos = timestampNanos;
            }
        }

        if (event.gsensor.sensor == SDL_SENSOR_GYRO) {
            // SDL uses a right-handed sensor coordinate system. Flip yaw to Latite's
            // camera convention, matching the GameInput path.
            Vec3 angularVelocity { event.gsensor.data[0], -event.gsensor.data[1], event.gsensor.data[2] };
            if (!std::isfinite(angularVelocity.x) || !std::isfinite(angularVelocity.y) ||
                !std::isfinite(angularVelocity.z)) {
                return;
            }
            if (gyroHandler) gyroHandler(id, angularVelocity, timestampNanos);
            return;
        }

        if (event.gsensor.sensor == SDL_SENSOR_ACCEL && accelerometerHandler) {
            Vec3 acceleration { event.gsensor.data[0], event.gsensor.data[1], event.gsensor.data[2] };
            if (std::isfinite(acceleration.x) && std::isfinite(acceleration.y) && std::isfinite(acceleration.z)) {
                accelerometerHandler(id, acceleration, timestampNanos);
            }
        }
    }

    void requestDeviceScan() {
        {
            std::scoped_lock lock { scanMutex };
            scanRequested = true;
        }
        scanCondition.notify_one();
    }

    void runInputLoop(std::stop_token stopToken) {
        auto nextDeviceScan = std::chrono::steady_clock::now() + DEVICE_SCAN_INTERVAL;
        while (!stopToken.stop_requested()) {
            SDL_UpdateGamepads();
            // This statically linked SDL instance has no event-loop consumer. The watch
            // has already delivered these events, so keep its private queue bounded.
            SDL_FlushEvents(SDL_EVENT_JOYSTICK_AXIS_MOTION, SDL_EVENT_JOYSTICK_UPDATE_COMPLETE);
            SDL_FlushEvents(SDL_EVENT_GAMEPAD_AXIS_MOTION, SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED);

            auto now = std::chrono::steady_clock::now();
            bool shouldScan = now >= nextDeviceScan;
            {
                std::scoped_lock lock { scanMutex };
                shouldScan |= std::exchange(scanRequested, false);
            }
            if (shouldScan) {
                scanDevices();
                nextDeviceScan = now + DEVICE_SCAN_INTERVAL;
            }

            std::unique_lock lock { scanMutex };
            scanCondition.wait_for(lock, INPUT_UPDATE_INTERVAL, [&] {
                return scanRequested || stopToken.stop_requested();
            });
        }
    }

    std::vector<DeviceState> deviceStatesLocked() const {
        std::vector<DeviceState> states;
        states.reserve(devices.size());
        for (auto const& device : devices) {
            states.push_back(device.state);
        }
        return states;
    }

    void notifyDeviceChange() {
        DeviceCallback deviceHandler;
        std::vector<DeviceState> states;
        {
            std::scoped_lock lock { mutex };
            if (!running.load(std::memory_order_relaxed)) return;
            deviceHandler = onDevice;
            states = deviceStatesLocked();
        }
        if (deviceHandler) deviceHandler(states);
    }

    void closeDevice(DeviceEntry& device) {
        if (!device.gamepad) return;
        SDL_SetGamepadSensorEnabled(device.gamepad, SDL_SENSOR_GYRO, false);
        if (device.state.hasAccelerometer) SDL_SetGamepadSensorEnabled(device.gamepad, SDL_SENSOR_ACCEL, false);
        SDL_CloseGamepad(device.gamepad);
        device.gamepad = nullptr;
    }

    void scanDevices() {
        if (!running.load(std::memory_order_acquire)) return;

        std::vector<DeviceEntry> disconnected;
        {
            std::scoped_lock lock { mutex };
            for (auto device = devices.begin(); device != devices.end();) {
                if (SDL_GamepadConnected(device->gamepad)) {
                    ++device;
                    continue;
                }
                disconnected.emplace_back(std::move(*device));
                device = devices.erase(device);
            }
        }
        for (auto& device : disconnected) {
            closeDevice(device);
        }

        bool devicesChanged = !disconnected.empty();
        int gamepadCount = 0;
        SDL_JoystickID* gamepads = SDL_GetGamepads(&gamepadCount);
        if (gamepads) {
            std::erase_if(unsupportedGamepads, [gamepads, gamepadCount](SDL_JoystickID unsupportedId) {
                return std::find(gamepads, gamepads + gamepadCount, unsupportedId) == gamepads + gamepadCount;
            });

            for (int index = 0; index < gamepadCount; ++index) {
                SDL_JoystickID instanceId = gamepads[index];
                if (std::ranges::find(unsupportedGamepads, instanceId) != unsupportedGamepads.end()) continue;

                bool alreadyOpen = false;
                {
                    std::scoped_lock lock { mutex };
                    alreadyOpen = std::ranges::any_of(devices, [instanceId](DeviceEntry const& entry) {
                        return entry.instanceId == instanceId;
                    });
                }
                if (alreadyOpen) continue;

                SDL_Gamepad* gamepad = SDL_OpenGamepad(instanceId);
                if (!gamepad) continue;
                if (!SDL_GamepadHasSensor(gamepad, SDL_SENSOR_GYRO)) {
                    unsupportedGamepads.push_back(instanceId);
                    SDL_CloseGamepad(gamepad);
                    continue;
                }
                if (!SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_GYRO, true)) {
                    SDL_CloseGamepad(gamepad);
                    continue;
                }

                bool hasAccelerometer = SDL_GamepadHasSensor(gamepad, SDL_SENSOR_ACCEL);
                hasAccelerometer &= SDL_SetGamepadSensorEnabled(gamepad, SDL_SENSOR_ACCEL, true);

                DeviceEntry entry;
                entry.gamepad = gamepad;
                entry.instanceId = instanceId;
                entry.state.id = deviceId(gamepad, instanceId);
                char const* gamepadName = SDL_GetGamepadName(gamepad);
                entry.state.name = L"SDL gyro controller";
                if (gamepadName && *gamepadName != '\0') entry.state.name = util::StrToWStr(gamepadName);
                entry.state.hasAccelerometer = hasAccelerometer;

                bool accepted = false;
                {
                    std::scoped_lock lock { mutex };
                    if (running.load(std::memory_order_relaxed)) {
                        for (auto const& other : devices) {
                            if (other.state.id != entry.state.id) continue;
                            entry.state.id += ":instance:" + std::to_string(instanceId);
                            break;
                        }
                        devices.emplace_back(std::move(entry));
                        accepted = true;
                    }
                }
                if (accepted)
                    devicesChanged = true;
                else
                    closeDevice(entry);
            }
            SDL_free(gamepads);
        }

        if (devicesChanged) notifyDeviceChange();
    }

    bool start(GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback, DeviceCallback deviceCallback) {
        stop();
        {
            std::scoped_lock lock { mutex };
            onGyro = std::move(gyroCallback);
            onAccelerometer = std::move(accelerometerCallback);
            onDevice = std::move(deviceCallback);
            running.store(true, std::memory_order_release);
        }

        if (!initializeSdl()) {
            std::scoped_lock lock { mutex };
            running.store(false, std::memory_order_release);
            onGyro = {};
            onAccelerometer = {};
            onDevice = {};
            return false;
        }

        scanDevices();
        inputThread = std::jthread([this](std::stop_token stopToken) {
            runInputLoop(stopToken);
        });

        return true;
    }

    void stop() {
        running.store(false, std::memory_order_release);

        if (inputThread.joinable()) {
            inputThread.request_stop();
            scanCondition.notify_one();
            inputThread.join();
        }
        {
            std::scoped_lock lock { scanMutex };
            scanRequested = false;
        }
        unsupportedGamepads.clear();

        if (sdlInitialized) {
            SDL_RemoveEventWatch(watchEvent, this);
        }

        std::vector<DeviceEntry> devicesToClose;
        {
            std::scoped_lock lock { mutex };
            devicesToClose = std::move(devices);
            devices.clear();
        }
        for (auto& device : devicesToClose) {
            closeDevice(device);
        }

        if (sdlInitialized) {
            SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
            sdlInitialized = false;
        }

        std::scoped_lock lock { mutex };
        onGyro = {};
        onAccelerometer = {};
        onDevice = {};
    }

    int64_t currentTimestampNanos(std::string const& id) const {
        std::scoped_lock lock { mutex };
        auto device = std::find_if(devices.begin(), devices.end(), [&id](DeviceEntry const& entry) {
            return entry.state.id == id;
        });
        return device == devices.end() ? 0 : device->lastTimestampNanos;
    }

    mutable std::mutex mutex;
    std::mutex scanMutex;
    std::condition_variable scanCondition;
    std::vector<DeviceEntry> devices;
    std::vector<SDL_JoystickID> unsupportedGamepads;
    GyroCallback onGyro;
    AccelerometerCallback onAccelerometer;
    DeviceCallback onDevice;
    std::jthread inputThread;
    std::atomic_bool running = false;
    bool scanRequested = false;
    bool sdlInitialized = false;
};

SdlGamepadGyroscope::SdlGamepadGyroscope()
    : impl(std::make_unique<Impl>()) {
}

SdlGamepadGyroscope::~SdlGamepadGyroscope() {
    stop();
}

bool SdlGamepadGyroscope::start(GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback,
                                DeviceCallback deviceCallback) {
    return impl->start(std::move(gyroCallback), std::move(accelerometerCallback), std::move(deviceCallback));
}

void SdlGamepadGyroscope::stop() {
    if (impl) impl->stop();
}

int64_t SdlGamepadGyroscope::currentTimestampNanos(std::string const& deviceId) const {
    return impl->currentTimestampNanos(deviceId);
}
