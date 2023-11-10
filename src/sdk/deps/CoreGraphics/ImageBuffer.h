#pragma once
#include <memory>

namespace SDK {
	namespace mce {
		class Blob {
		public:
			// using default delete for now, it will not be constructible
			std::unique_ptr<unsigned char[0]> mBlob;
			size_t size;

			Blob(): size(0) {

			}

			Blob(size_t size): size(size) {
				mBlob = std::make_unique<unsigned char[]>(size);
			}
		};
	}

	// CoreGraphics
	namespace cg {
		struct ImageDescription {
			float mWidth = 0.f;
			float mHeight = 0.f;
			int mTextureFormat = 0;
			uint8_t mColorSpace = 0;
			bool mIsCubemap = false;
			int mArraySize = 0;
		};

		static_assert(sizeof(ImageDescription) == 0x14);

		class ImageBuffer {
		public:
			mce::Blob mStorage{ 0 };
			ImageDescription mImageDescription{};

			ImageBuffer(int imageWidth, int imageHeight) {
				mStorage = mce::Blob(imageWidth * imageHeight);
				mImageDescription.mWidth = (float)imageWidth;
				mImageDescription.mHeight = (float)imageHeight;
			}

			~ImageBuffer() {}
		};
	}
};