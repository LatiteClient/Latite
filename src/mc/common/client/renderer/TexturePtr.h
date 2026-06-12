#pragma once
#include <memory>

#include "mc/deps/core/resource/ResourceLocation.h"

namespace SDK {
    class BedrockTextureData {
    private:
        void* vtable; // I don't think these are owned, so won't free them off the memory
        //void* idk2;

    public:
        //std::shared_ptr<ResourceLocation> resource;

    private:
        void* empty[10];

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
