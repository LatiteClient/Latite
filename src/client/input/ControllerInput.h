#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "util/LMath.h"

namespace controller_input {
    // Windows virtual-key codes occupy one byte. Controller buttons are stored
    // above that range so existing integer keybind configs remain compatible.
    inline constexpr int KEY_BASE = 0x100;
    inline constexpr int BUTTON_COUNT = 28;

    [[nodiscard]] constexpr bool isButton(int key) {
        return key >= KEY_BASE && key < KEY_BASE + BUTTON_COUNT;
    }

    [[nodiscard]] constexpr int keyFromButtonIndex(int index) {
        return KEY_BASE + index;
    }

    [[nodiscard]] std::string_view buttonName(int key);
    [[nodiscard]] int buttonFromName(std::string_view name);
}

class ControllerInput final {
public:
    struct SensorDeviceState {
        std::string id;
        std::wstring name;
        bool hasAccelerometer = false;
    };

    using GyroCallback = std::function<void(std::string const&, Vec3 const&, int64_t)>;
    using AccelerometerCallback = std::function<void(std::string const&, Vec3 const&, int64_t)>;
    using SensorDeviceCallback = std::function<void(std::vector<SensorDeviceState> const&)>;

    ControllerInput();
    ~ControllerInput();

    ControllerInput(ControllerInput const&) = delete;
    ControllerInput& operator=(ControllerInput const&) = delete;

    [[nodiscard]] bool start();
    void stop();
    void update();

    [[nodiscard]] bool startSensors(GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback,
                                    SensorDeviceCallback deviceCallback);
    void stopSensors();
    [[nodiscard]] int64_t currentSensorTimestampNanos(std::string const& deviceId) const;
    [[nodiscard]] std::optional<Vec2> rightStick(std::string const& deviceId = {}) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
