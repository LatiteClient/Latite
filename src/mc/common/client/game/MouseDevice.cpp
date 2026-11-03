#include "MouseDevice.h"

SDK::MouseDevice* SDK::MouseDevice::get() {
    static auto mouse = reinterpret_cast<MouseDevice*>(Signatures::Misc::mouseDevice.result);
    return mouse;
}
