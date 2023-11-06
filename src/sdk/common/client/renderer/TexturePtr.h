#pragma once
#include <memory>
#include <string>

namespace SDK {
    class ResourceLocation {
    public:
        alignas(8) int type;
        std::string txt;

        ResourceLocation(std::string const& text, int type) {
            txt = text;
            this->type = type;
        }

        ~ResourceLocation() = default;
    private:
        uint64_t mPathHash = 0;
        uint64_t mFullHash = 0;
    };

    class BedrockTextureData {
    private:
        void* idk; // I don't think these are owned, so won't free them off the memory
        void* idk2;

    public:
        std::shared_ptr<ResourceLocation> resource;

    private:
        void* empty[4];

        ~BedrockTextureData() {
        }
    };

    class TexturePtr {
    public:
        std::shared_ptr<BedrockTextureData> textureData;
        std::shared_ptr<ResourceLocation> resourceLocation;

        void clearTexture() {
            resourceLocation.reset();
            textureData.reset();
        }

        ~TexturePtr() {
        }
    };
}