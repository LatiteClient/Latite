#pragma once

#include "mc/deps/core/file/Path.h"

namespace SDK {
    class ImagePickingCallback {
    public:
        virtual ~ImagePickingCallback() = default;
        virtual void onImagePicked(Core::Path const& path) = 0;
        virtual void onImagePickCancelled() = 0;
    };
}
