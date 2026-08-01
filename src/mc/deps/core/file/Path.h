#pragma once

#include <string>
#include <utility>

namespace SDK::Core {
    // Core::Path's only data member comes from PathBuffer<std::string>.
    class Path {
    public:
        std::string value;

        Path() = default;

        explicit Path(std::string path) { value = std::move(path); }
    };

    static_assert(sizeof(Path) == sizeof(std::string));
}
