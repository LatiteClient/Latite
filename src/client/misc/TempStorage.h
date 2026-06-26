#pragma once

#include <filesystem>

namespace LatiteTemp {
    std::filesystem::path resolvePath(std::filesystem::path const& relative);
    void cleanup();
}
