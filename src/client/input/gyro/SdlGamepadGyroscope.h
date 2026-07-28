#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "util/LMath.h"

class SdlGamepadGyroscope final {
public:
    struct DeviceState {
        std::string id;
        std::wstring name;
        bool hasAccelerometer = false;
    };

    using GyroCallback = std::function<void(std::string const&, Vec3 const&, int64_t)>;
    using AccelerometerCallback = std::function<void(std::string const&, Vec3 const&, int64_t)>;
    using DeviceCallback = std::function<void(std::vector<DeviceState> const&)>;

    SdlGamepadGyroscope();
    ~SdlGamepadGyroscope();

    SdlGamepadGyroscope(SdlGamepadGyroscope const&) = delete;
    SdlGamepadGyroscope& operator=(SdlGamepadGyroscope const&) = delete;

    bool start(GyroCallback gyroCallback, AccelerometerCallback accelerometerCallback, DeviceCallback deviceCallback);
    void stop();

    [[nodiscard]] int64_t currentTimestampNanos(std::string const& deviceId) const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
