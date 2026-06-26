#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace SDK {
    namespace Core {
        template<typename T>
        class PathBuffer {
        public:
            T value;

            PathBuffer* operator->() { return this; }

            PathBuffer const* operator->() const { return this; }
        };
    }

    enum class ResourceFileSystem : int {
        UserPackage = 0,
        AppPackage = 1,
        Raw = 2,
        RawPersistent = 3,
        SettingsDir = 4,
        ExternalDir = 5,
        ServerPackage = 6,
        DataDir = 7,
        UserDir = 8,
        ScreenshotsDir = 9,
        StoreCache = 10,
        MaterialsDir = 11,
        Invalid = 12,
    };

    class ResourceLocation {
    public:
        ResourceFileSystem mFileSystem = ResourceFileSystem::UserPackage;
        Core::PathBuffer<std::string> mPath;

        ResourceLocation() = default;

        explicit ResourceLocation(ResourceFileSystem fileSystem)
            : mFileSystem(fileSystem) {}

        ResourceLocation(std::string const& path, ResourceFileSystem fileSystem)
            : mFileSystem(fileSystem) {
            mPath->value = path;
        }

        ResourceLocation(std::string const& path, int fileSystem)
            : ResourceLocation(path, static_cast<ResourceFileSystem>(fileSystem)) {}

    private:
        std::uint64_t mPathHash = 0;
        std::uint64_t mFullHash = 0;
    };

    static_assert(sizeof(ResourceLocation) == 0x38);
    static_assert(offsetof(ResourceLocation, mPath) == 0x8);
}
