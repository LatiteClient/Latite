#pragma once
#include <memory>
#include <string>

namespace SDK {
    class ResourceLocation {
    public:
        alignas(8) int type;
        std::string txt;

        ResourceLocation(const char* text, int type) {
            txt = text;
            this->type = type;
        }

        ~ResourceLocation() = default;
    };

    class TexturePtr {
    public:
        std::shared_ptr<class BedrockTextureData> textureData;
        std::shared_ptr<ResourceLocation> resourceLocation;
    };
}