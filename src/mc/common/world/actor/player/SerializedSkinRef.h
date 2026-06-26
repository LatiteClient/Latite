#pragma once
#include "mc/Util.h"
#include <cstdint>
#include <string>

namespace SDK {
    class SkinImageBlob {
    public:
        unsigned char const* data() const { return dataPtr; }
        size_t size() const { return dataSize; }

    private:
        void* deleter = nullptr;
        unsigned char* dataPtr = nullptr;
        size_t dataSize = 0;
    };

    class SkinImage {
    public:
        bool hasRgbaBytes() const {
            if (!bytes.data() || width == 0 || height == 0) return false;
            if (width > 4096 || height > 4096) return false;
            auto expectedSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4u;
            return bytes.size() >= expectedSize;
        }

        int imageFormat = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 0;
        uint8_t usage = 0;
        uint8_t pad[7] {};
        SkinImageBlob bytes;
    };

    class SerializedSkinRef {
    public:
        std::string const* getId() const {
            auto wrapper = *reinterpret_cast<uintptr_t const*>(this);
            if (!wrapper) return nullptr;

            return reinterpret_cast<std::string const*>(wrapper);
        }

        std::string const* getName() const {
            auto wrapper = *reinterpret_cast<uintptr_t const*>(this);
            if (!wrapper) return nullptr;

            return reinterpret_cast<std::string const*>(wrapper + 0x40);
        }

        SkinImage const* getSkinImage() const {
            auto wrapper = *reinterpret_cast<uintptr_t const*>(this);
            if (!wrapper) return nullptr;

            return reinterpret_cast<SkinImage const*>(wrapper + 0xA0);
        }

        bool isValid() const {
            auto name = getName();
            return name && !name->empty();
        }

    private:
        void* skinImpl = nullptr;
        void* refCount = nullptr;
    };

}
