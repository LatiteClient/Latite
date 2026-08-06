#pragma once

#include <cstdint>
#include <string>

namespace SDK {
    class SoundPlayerInterface {
    public:
        std::uint64_t playUI(std::string const& name, float volume, float pitch);
    };
}
