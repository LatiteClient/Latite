#include "pch.h"
#include "ControllerInput.h"

#include "client/Latite.h"
#include "client/event/Eventing.h"
#include "client/event/events/KeyUpdateEvent.h"
#include "client/input/Keyboard.h"
#include "client/script/PluginManager.h"
#include "mc/common/client/game/GameCore.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cctype>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace {
    constexpr auto DEVICE_SCAN_INTERVAL = std::chrono::milliseconds(250);
    constexpr auto INPUT_UPDATE_INTERVAL = std::chrono::milliseconds(1);
    constexpr auto SENSOR_START_TIMEOUT = std::chrono::seconds(1);
    constexpr Sint16 TRIGGER_BUTTON_THRESHOLD = 16000;
    constexpr int LEFT_TRIGGER_BUTTON = SDL_GAMEPAD_BUTTON_COUNT;
    constexpr int RIGHT_TRIGGER_BUTTON = SDL_GAMEPAD_BUTTON_COUNT + 1;

    static_assert(controller_input::BUTTON_COUNT == SDL_GAMEPAD_BUTTON_COUNT + 2);

    constexpr std::array<std::string_view, controller_input::BUTTON_COUNT> BUTTON_NAMES = {
        "Controller A / Cross",
        "Controller B / Circle",
        "Controller X / Square",
        "Controller Y / Triangle",
        "Controller Back / View / Create",
        "Controller Guide / Home",
        "Controller Start / Menu / Options",
        "Controller Left Stick",
        "Controller Right Stick",
        "Controller LB / L1",
        "Controller RB / R1",
        "Controller D-Pad Up",
        "Controller D-Pad Down",
        "Controller D-Pad Left",
        "Controller D-Pad Right",
        "Controller Misc 1",
        "Controller Right Paddle 1",
        "Controller Left Paddle 1",
        "Controller Right Paddle 2",
        "Controller Left Paddle 2",
        "Controller Touchpad",
        "Controller Misc 2",
        "Controller Misc 3",
        "Controller Misc 4",
        "Controller Misc 5",
        "Controller Misc 6",
        "Controller LT / L2",
        "Controller RT / R2",
    };

    std::string lowercase(std::string_view value) {
        std::string result(value);
        std::ranges::transform(result, result.begin(), [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        });
        return result;
    }

    std::string deviceId(SDL_Gamepad* gamepad, SDL_JoystickID instanceId) {
        char const* path = SDL_GetGamepadPath(gamepad);
        if (path && *path != '\0') return "sdl:path:" + std::string(path);

        char const* serial = SDL_GetGamepadSerial(gamepad);
        if (serial && *serial != '\0') return "sdl:serial:" + std::string(serial);

        char const* name = SDL_GetGamepadName(gamepad);
        std::string fallbackName = name && *name != '\0' ? name : "controller";
        return "sdl:session:" + fallbackName + ":" + std::to_string(instanceId);
    }
}

std::string_view controller_input::buttonName(int key) {
    if (!isButton(key)) return {};
    return BUTTON_NAMES[static_cast<std::size_t>(key - KEY_BASE)];
}

int controller_input::buttonFromName(std::string_view name) {
    std::string normalizedName = lowercase(name);
    for (std::size_t index = 0; index < BUTTON_NAMES.size(); ++index) {
        if (lowercase(BUTTON_NAMES[index]) == normalizedName) {
            return keyFromButtonIndex(static_cast<int>(index));
        }
    }
    return 0;
}

struct ControllerInput::Impl {
    struct SensorSubscription {
        std::mutex invocationMutex;
        GyroCallback onGyro;
        AccelerometerCallback onAccelerometer;
        SensorDeviceCallback onDevice;
        bool active = true;
    };

    struct DeviceEntry {
        SDL_Gamepad* gamepad = nullptr;
        SDL_JoystickID instanceId = 0;
        SensorDeviceState sensorState;
        uint32_t buttons = 0;
        int64_t lastTimestampNanos = 0;
        int64_t lastAccelerometerTimestampNanos = 0;
        bool buttonStateInitialized = false;
        bool hasGyroscope = false;
        bool hasAccelerometer = false;
        bool gyroscopeEnabled = false;
        bool accelerometerEnabled = false;
    };

    struct SensorUpdate {
        SDL_Gamepad* gamepad = nullptr;
        bool hasGyroscope = false;
        bool hasAccelerometer = false;
        bool gyroscopeEnabled = false;
        bool accelerometerEnabled = false;
    };

    static bool SDLCALL watchEvent(void* context, SDL_Event* event) {
        if (context && event) static_cast<Impl*>(context)->handleEvent(*event);
        return true;
    }

    bool start() {
        stop();

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

        running.store(true, std::memory_order_release);
        scanDevices();
        refreshButtonStates();
        applySensorRequest();
        inputThread = std::jthread([this](std::stop_token stopToken) {
            runInputLoop(stopToken);
        });
        return true;
    }

    void stop() {
        running.store(false, std::memory_order_release);
        sensorApplyCondition.notify_all();
        requestInputUpdate();

        auto oldSubscription = takeSensorSubscription();
        deactivateSubscription(oldSubscription);

        if (inputThread.joinable()) {
            inputThread.request_stop();
            inputCondition.notify_one();
            inputThread.join();
        }

        if (sdlInitialized) SDL_RemoveEventWatch(watchEvent, this);

        std::vector<DeviceEntry> devicesToClose;
        {
            std::scoped_lock lock { mutex };
            devicesToClose = std::move(devices);
            devices.clear();
            sensorsRequested = false;
            previousButtons = 0;
            suppressedButtons = 0;
            sensorRequestVersion = 0;
            sensorAppliedVersion = 0;
        }
        for (auto& device : devicesToClose) {
            closeDevice(device);
        }

        {
            std::scoped_lock lock { inputMutex };
            scanRequested = false;
        }

        if (sdlInitialized) {
            SDL_QuitSubSystem(SDL_INIT_GAMEPAD);
            sdlInitialized = false;
        }
    }

    void update() {
        uint32_t currentButtons = 0;
        uint32_t changedButtons = 0;
        {
            std::scoped_lock lock { mutex };
            if (!running.load(std::memory_order_relaxed)) return;

            for (auto& device : devices) {
                if (!device.buttonStateInitialized) {
                    suppressedButtons |= device.buttons;
                    device.buttonStateInitialized = true;
                }
                currentButtons |= device.buttons;
            }

            auto* gameCore = SDK::GameCore::get();
            bool gameFocused = gameCore && GetForegroundWindow() == gameCore->hwnd;
            if (!gameFocused) {
                suppressedButtons |= currentButtons;
                currentButtons = 0;
            } else {
                suppressedButtons &= currentButtons;
                currentButtons &= ~suppressedButtons;
            }

            changedButtons = previousButtons ^ currentButtons;
            previousButtons = currentButtons;
        }

        while (changedButtons != 0) {
            unsigned int buttonIndex = std::countr_zero(changedButtons);
            uint32_t buttonMask = uint32_t { 1 } << buttonIndex;
            changedButtons &= ~buttonMask;

            int key = controller_input::keyFromButtonIndex(static_cast<int>(buttonIndex));
            bool isDown = (currentButtons & buttonMask) != 0;
            Latite::getKeyboard().setControllerButtonState(key, isDown);

            PluginManager::Event::Value downValue { L"isDown" };
            downValue.val = isDown;
            PluginManager::Event::Value characterValue { L"keyAsChar" };
            characterValue.val = std::wstring {};
            PluginManager::Event::Value keyCodeValue { L"keyCode" };
            keyCodeValue.val = static_cast<double>(key);
            PluginManager::Event scriptEvent { L"key-press", { downValue, characterValue, keyCodeValue }, true };
            if (Latite::getPluginManager().dispatchEvent(scriptEvent)) continue;

            KeyUpdateEvent event { key, isDown };
            Eventing::get().dispatch(event);
        }
    }

    bool startSensors(GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback,
                      SensorDeviceCallback deviceCallback) {
        stopSensors();

        auto subscription = std::make_shared<SensorSubscription>();
        subscription->onGyro = std::move(gyroCallback);
        subscription->onAccelerometer = std::move(accelerometerCallback);
        subscription->onDevice = std::move(deviceCallback);

        uint64_t requestedVersion = 0;
        {
            std::scoped_lock lock { mutex };
            if (!running.load(std::memory_order_relaxed)) return false;
            sensorSubscription = std::move(subscription);
            sensorsRequested = true;
            requestedVersion = ++sensorRequestVersion;
        }
        requestInputUpdate();

        std::unique_lock lock { mutex };
        sensorApplyCondition.wait_for(lock, SENSOR_START_TIMEOUT, [&] {
            return !running.load(std::memory_order_relaxed) || sensorAppliedVersion >= requestedVersion;
        });
        return running.load(std::memory_order_relaxed);
    }

    void stopSensors() {
        std::shared_ptr<SensorSubscription> oldSubscription;
        {
            std::scoped_lock lock { mutex };
            oldSubscription = std::exchange(sensorSubscription, {});
            if (sensorsRequested) {
                sensorsRequested = false;
                ++sensorRequestVersion;
            }
        }
        deactivateSubscription(oldSubscription);
        requestInputUpdate();
    }

    int64_t currentSensorTimestampNanos(std::string const& id) const {
        std::scoped_lock lock { mutex };
        auto device = std::find_if(devices.begin(), devices.end(), [&id](DeviceEntry const& entry) {
            return entry.sensorState.id == id;
        });
        return device == devices.end() ? 0 : device->lastTimestampNanos;
    }

    void handleEvent(SDL_Event const& event) {
        if (!running.load(std::memory_order_acquire)) return;

        if (event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED ||
            event.type == SDL_EVENT_GAMEPAD_REMAPPED) {
            requestDeviceScan();
            return;
        }
        if (event.type != SDL_EVENT_GAMEPAD_SENSOR_UPDATE) return;

        std::shared_ptr<SensorSubscription> subscription;
        std::string id;
        int64_t timestampNanos = static_cast<int64_t>(event.gsensor.sensor_timestamp);
        if (timestampNanos <= 0) timestampNanos = static_cast<int64_t>(event.gsensor.timestamp);

        {
            std::scoped_lock lock { mutex };
            auto device = std::find_if(devices.begin(), devices.end(), [&event](DeviceEntry const& entry) {
                return entry.instanceId == event.gsensor.which;
            });
            if (device == devices.end()) return;

            if (event.gsensor.sensor == SDL_SENSOR_GYRO) {
                if (!device->gyroscopeEnabled || timestampNanos <= device->lastTimestampNanos) return;
                device->lastTimestampNanos = timestampNanos;
            } else if (event.gsensor.sensor == SDL_SENSOR_ACCEL) {
                if (!device->accelerometerEnabled || timestampNanos <= device->lastAccelerometerTimestampNanos) return;
                device->lastAccelerometerTimestampNanos = timestampNanos;
            } else {
                return;
            }

            id = device->sensorState.id;
            subscription = sensorSubscription;
        }
        if (!subscription) return;

        std::scoped_lock invocationLock { subscription->invocationMutex };
        if (!subscription->active) return;

        if (event.gsensor.sensor == SDL_SENSOR_GYRO && subscription->onGyro) {
            Vec3 angularVelocity { event.gsensor.data[0], event.gsensor.data[1], event.gsensor.data[2] };
            if (std::isfinite(angularVelocity.x) && std::isfinite(angularVelocity.y) &&
                std::isfinite(angularVelocity.z)) {
                subscription->onGyro(id, angularVelocity, timestampNanos);
            }
            return;
        }

        if (event.gsensor.sensor == SDL_SENSOR_ACCEL && subscription->onAccelerometer) {
            Vec3 acceleration { event.gsensor.data[0], event.gsensor.data[1], event.gsensor.data[2] };
            if (std::isfinite(acceleration.x) && std::isfinite(acceleration.y) && std::isfinite(acceleration.z)) {
                subscription->onAccelerometer(id, acceleration, timestampNanos);
            }
        }
    }

    void runInputLoop(std::stop_token stopToken) {
        auto nextDeviceScan = std::chrono::steady_clock::now() + DEVICE_SCAN_INTERVAL;
        while (!stopToken.stop_requested()) {
            SDL_UpdateGamepads();
            refreshButtonStates();
            // The event watch has already delivered these events, so keep the
            // statically linked SDL instance's private queue bounded.
            SDL_FlushEvents(SDL_EVENT_JOYSTICK_AXIS_MOTION, SDL_EVENT_JOYSTICK_UPDATE_COMPLETE);
            SDL_FlushEvents(SDL_EVENT_GAMEPAD_AXIS_MOTION, SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED);

            auto now = std::chrono::steady_clock::now();
            bool shouldScan = now >= nextDeviceScan;
            {
                std::scoped_lock lock { inputMutex };
                shouldScan |= std::exchange(scanRequested, false);
            }
            if (shouldScan) {
                scanDevices();
                nextDeviceScan = now + DEVICE_SCAN_INTERVAL;
            }

            bool shouldApplySensors = false;
            {
                std::scoped_lock lock { mutex };
                shouldApplySensors = sensorAppliedVersion < sensorRequestVersion;
            }
            if (shouldApplySensors) applySensorRequest();

            std::unique_lock lock { inputMutex };
            inputCondition.wait_for(lock, INPUT_UPDATE_INTERVAL, [&] {
                return scanRequested || stopToken.stop_requested();
            });
        }
    }

    void refreshButtonStates() {
        std::vector<std::pair<SDL_Gamepad*, uint32_t>> states;
        {
            std::scoped_lock lock { mutex };
            states.reserve(devices.size());
            for (auto const& device : devices) {
                states.emplace_back(device.gamepad, 0);
            }
        }

        for (auto& [gamepad, buttons] : states) {
            for (int button = 0; button < SDL_GAMEPAD_BUTTON_COUNT; ++button) {
                if (SDL_GetGamepadButton(gamepad, static_cast<SDL_GamepadButton>(button))) {
                    buttons |= uint32_t { 1 } << button;
                }
            }
            if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > TRIGGER_BUTTON_THRESHOLD) {
                buttons |= uint32_t { 1 } << LEFT_TRIGGER_BUTTON;
            }
            if (SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > TRIGGER_BUTTON_THRESHOLD) {
                buttons |= uint32_t { 1 } << RIGHT_TRIGGER_BUTTON;
            }
        }

        std::scoped_lock lock { mutex };
        for (auto& device : devices) {
            auto state = std::ranges::find_if(states, [&device](auto const& entry) {
                return entry.first == device.gamepad;
            });
            if (state != states.end()) device.buttons = state->second;
        }
    }

    void requestDeviceScan() {
        {
            std::scoped_lock lock { inputMutex };
            scanRequested = true;
        }
        inputCondition.notify_one();
    }

    void requestInputUpdate() { inputCondition.notify_one(); }

    bool scanDevices() {
        if (!running.load(std::memory_order_acquire)) return false;

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
            for (int index = 0; index < gamepadCount; ++index) {
                SDL_JoystickID instanceId = gamepads[index];

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

                DeviceEntry entry;
                entry.gamepad = gamepad;
                entry.instanceId = instanceId;
                entry.sensorState.id = deviceId(gamepad, instanceId);
                char const* gamepadName = SDL_GetGamepadName(gamepad);
                entry.sensorState.name = L"SDL controller";
                if (gamepadName && *gamepadName != '\0') entry.sensorState.name = util::StrToWStr(gamepadName);
                entry.hasGyroscope = SDL_GamepadHasSensor(gamepad, SDL_SENSOR_GYRO);
                entry.hasAccelerometer = SDL_GamepadHasSensor(gamepad, SDL_SENSOR_ACCEL);
                entry.sensorState.hasAccelerometer = entry.hasAccelerometer;

                bool accepted = false;
                {
                    std::scoped_lock lock { mutex };
                    if (running.load(std::memory_order_relaxed)) {
                        for (auto const& other : devices) {
                            if (other.sensorState.id != entry.sensorState.id) continue;
                            entry.sensorState.id += ":instance:" + std::to_string(instanceId);
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

        if (devicesChanged) {
            std::scoped_lock lock { mutex };
            ++sensorRequestVersion;
        }
        return devicesChanged;
    }

    void applySensorRequest() {
        bool requested = false;
        uint64_t requestVersion = 0;
        std::vector<SensorUpdate> updates;
        {
            std::scoped_lock lock { mutex };
            requested = sensorsRequested;
            requestVersion = sensorRequestVersion;
            updates.reserve(devices.size());
            for (auto const& device : devices) {
                updates.push_back({ device.gamepad, device.hasGyroscope, device.hasAccelerometer,
                                    device.gyroscopeEnabled, device.accelerometerEnabled });
            }
        }

        for (auto& update : updates) {
            bool wantGyroscope = requested && update.hasGyroscope;
            if (wantGyroscope != update.gyroscopeEnabled) {
                if (SDL_SetGamepadSensorEnabled(update.gamepad, SDL_SENSOR_GYRO, wantGyroscope))
                    update.gyroscopeEnabled = wantGyroscope;
                else if (wantGyroscope)
                    update.gyroscopeEnabled = false;
            }

            bool wantAccelerometer = requested && update.gyroscopeEnabled && update.hasAccelerometer;
            if (wantAccelerometer != update.accelerometerEnabled) {
                if (SDL_SetGamepadSensorEnabled(update.gamepad, SDL_SENSOR_ACCEL, wantAccelerometer))
                    update.accelerometerEnabled = wantAccelerometer;
                else if (wantAccelerometer)
                    update.accelerometerEnabled = false;
            }
        }

        std::shared_ptr<SensorSubscription> subscription;
        std::vector<SensorDeviceState> sensorDevices;
        {
            std::scoped_lock lock { mutex };
            for (auto& device : devices) {
                auto update = std::ranges::find_if(updates, [&device](SensorUpdate const& entry) {
                    return entry.gamepad == device.gamepad;
                });
                if (update == updates.end()) continue;
                device.gyroscopeEnabled = update->gyroscopeEnabled;
                device.accelerometerEnabled = update->accelerometerEnabled;
                device.sensorState.hasAccelerometer = update->accelerometerEnabled;
                if (device.gyroscopeEnabled) sensorDevices.push_back(device.sensorState);
            }
            if (sensorsRequested) subscription = sensorSubscription;
        }
        notifySensorDevices(subscription, sensorDevices);
        {
            std::scoped_lock lock { mutex };
            sensorAppliedVersion = std::max(sensorAppliedVersion, requestVersion);
        }
        sensorApplyCondition.notify_all();
    }

    static void notifySensorDevices(std::shared_ptr<SensorSubscription> const& subscription,
                                    std::vector<SensorDeviceState> const& devices) {
        if (!subscription) return;
        std::scoped_lock invocationLock { subscription->invocationMutex };
        if (subscription->active && subscription->onDevice) subscription->onDevice(devices);
    }

    std::shared_ptr<SensorSubscription> takeSensorSubscription() {
        std::scoped_lock lock { mutex };
        sensorsRequested = false;
        return std::exchange(sensorSubscription, {});
    }

    static void deactivateSubscription(std::shared_ptr<SensorSubscription> const& subscription) {
        if (!subscription) return;
        std::scoped_lock invocationLock { subscription->invocationMutex };
        subscription->active = false;
        subscription->onGyro = {};
        subscription->onAccelerometer = {};
        subscription->onDevice = {};
    }

    static void closeDevice(DeviceEntry& device) {
        if (!device.gamepad) return;
        if (device.gyroscopeEnabled) SDL_SetGamepadSensorEnabled(device.gamepad, SDL_SENSOR_GYRO, false);
        if (device.accelerometerEnabled) SDL_SetGamepadSensorEnabled(device.gamepad, SDL_SENSOR_ACCEL, false);
        SDL_CloseGamepad(device.gamepad);
        device.gamepad = nullptr;
    }

    mutable std::mutex mutex;
    std::mutex inputMutex;
    std::condition_variable inputCondition;
    std::condition_variable sensorApplyCondition;
    std::vector<DeviceEntry> devices;
    std::shared_ptr<SensorSubscription> sensorSubscription;
    std::jthread inputThread;
    std::atomic_bool running = false;
    uint64_t sensorRequestVersion = 0;
    uint64_t sensorAppliedVersion = 0;
    uint32_t previousButtons = 0;
    uint32_t suppressedButtons = 0;
    bool sensorsRequested = false;
    bool scanRequested = false;
    bool sdlInitialized = false;
};

ControllerInput::ControllerInput()
    : impl(std::make_unique<Impl>()) {
}

ControllerInput::~ControllerInput() {
    stop();
}

bool ControllerInput::start() {
    return impl && impl->start();
}

void ControllerInput::stop() {
    if (impl) impl->stop();
}

void ControllerInput::update() {
    if (impl) impl->update();
}

bool ControllerInput::startSensors(GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback,
                                   SensorDeviceCallback deviceCallback) {
    return impl &&
           impl->startSensors(std::move(gyroCallback), std::move(accelerometerCallback), std::move(deviceCallback));
}

void ControllerInput::stopSensors() {
    if (impl) impl->stopSensors();
}

int64_t ControllerInput::currentSensorTimestampNanos(std::string const& deviceId) const {
    return impl ? impl->currentSensorTimestampNanos(deviceId) : 0;
}
