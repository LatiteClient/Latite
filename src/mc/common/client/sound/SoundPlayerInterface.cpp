#include "pch.h"
#include "SoundPlayerInterface.h"

std::uint64_t SDK::SoundPlayerInterface::playUI(std::string const& name, float volume, float pitch) {
    return memory::callVirtual<std::uint64_t, std::string const&, float, float>(this, 3, name, volume, pitch);
}
