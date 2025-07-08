#include "pch.h"

MouseDevice* MouseDevice::get() {
    return Signatures::Misc::mouseDevice.as_ptr<MouseDevice>();
}
