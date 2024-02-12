#pragma once
#include <memory>

namespace SDK {
	namespace mce {
		class Blob {
		public:
			// FIXME: Theres no way any of this is right
			static void defaultDeleter(unsigned char* mine) {
				// free(mine); I dont even know at this point, this will 100% cause a memory leak
			}

			struct Deleter {
				void(*m_func)(unsigned char*);

				void operator()(auto* item) {
					m_func((unsigned char*)item);
				}

			};
			std::unique_ptr<unsigned char*, Deleter> mBlob;
			size_t size;

			Blob(): size(0) {
				std::default_delete mydelete = std::default_delete<unsigned char*>();

			}

			Blob(size_t size): size(size) {
				unsigned char* actualBlob = (unsigned char*)malloc(size);
				memset(actualBlob, 0xFF, size);
				const auto mblobsize = sizeof(mBlob);

				unsigned char** ptrToBlob = new unsigned char*;
				ptrToBlob = &actualBlob;

				mBlob = std::unique_ptr<unsigned char*, Deleter>(ptrToBlob, Deleter{ defaultDeleter });
			}

			~Blob() = default; // I think this will do everything????
		};
	}

	// CoreGraphics
	namespace cg {
		struct ImageDescription {
			float mWidth = 0.f;
			float mHeight = 0.f;
			int mTextureFormat = 1;
			uint8_t mColorSpace = 16;
			bool mIsCubemap = false;
			int mArraySize = 0;
		};

		static_assert(sizeof(ImageDescription) == 0x14);

		class ImageBuffer {
		public:
			mce::Blob mStorage;
			ImageDescription mImageDescription{};

			ImageBuffer(int imageWidth, int imageHeight) : mStorage{ (imageWidth * imageHeight) * 4 /*Im assuming its ARGB/RGBA so 4 bytes per pixel?*/} {
				mImageDescription.mWidth = (float)imageWidth;
				mImageDescription.mHeight = (float)imageHeight;
				mImageDescription.mArraySize = (int)mStorage.size;
			}

			~ImageBuffer() {}
		};
	}
};