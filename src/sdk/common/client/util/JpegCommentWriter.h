#pragma once
#include <sdk/common/client/renderer/MaterialPtr.h>

namespace SDK {
	namespace cg {
		class ImageBuffer;
	}
	
	class JpegCommentWriter {
		[[maybe_unused]] char pad[0xA8];
	public:

		// ui_texture_and_color
		MaterialPtr material;
	private:
		//size_t trap[69] = { 0xdeadc0de };
	public:

		JpegCommentWriter(MaterialPtr* mat) : material(*mat) {}

		void _drawImage(ScreenContext* scn, cg::ImageBuffer* image);
	};
}