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
        std::shared_ptr<class BedrockTextureData> textureData;

        void clearTexture() {
            textureData.reset();
        }

        virtual ~TexturePtr() {
        }
    };

    class TexturePtr_New : public TexturePtr {
    public:
        std::shared_ptr<ResourceLocation> resourceLocation;
    };

    class TexturePtr_Old : public TexturePtr {
    public:
        ResourceLocation resourceLocation = ResourceLocation("", 0);
    private:
        [[maybe_unused]] char pad[20];//temp fix
    public:
        TexturePtr_Old() {
            memset(this, 0x0, sizeof(TexturePtr_Old));
            resourceLocation = ResourceLocation("", 0);
        }

        ~TexturePtr_Old() {
        }
    };
}