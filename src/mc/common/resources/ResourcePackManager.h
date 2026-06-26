#pragma once

#include <string>

#include "mc/deps/core/resource/ResourceLocation.h"
#include "util/memory.h"

namespace SDK {
    class ResourcePackManager {
    public:
        bool load(ResourceLocation const& resourceLocation, std::string& resourceStream) const {
            return memory::callVirtual<bool, ResourceLocation const&, std::string&>(
                const_cast<ResourcePackManager*>(this), 1, resourceLocation, resourceStream);
        }
    };
}
