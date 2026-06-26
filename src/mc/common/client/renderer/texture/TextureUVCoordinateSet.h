#pragma once

#include <cstdint>

#include "mc/deps/core/resource/ResourceLocation.h"

namespace SDK {
    struct TextureUVCoordinateSet {
        float weight;
        float u0;
        float v0;
        float u1;
        float v1;
        std::uint16_t texSizeW;
        std::uint16_t texSizeH;
        ResourceLocation sourceFileLocation;
        std::uint8_t isotropicFaceData;
        std::uint8_t padding;
        std::int16_t textureSetTranslationIndex;
        std::uint16_t pbrTextureDataHandle;
    };

    static_assert(sizeof(TextureUVCoordinateSet) == 0x58);
    static_assert(offsetof(TextureUVCoordinateSet, sourceFileLocation) == 0x18);
}
