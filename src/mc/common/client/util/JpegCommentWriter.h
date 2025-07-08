#pragma once
#include <mc/common/client/renderer/MaterialPtr.h>

namespace SDK {
	namespace cg {
		class ImageBuffer;
	}
	
	class JpegCommentWriter {
		[[maybe_unused]] size_t trap[0xA8] = {0xdeadc0de};
	public:

		// ui_texture_and_color
		
		CLASS_FIELD(MaterialPtr, material, 0xa8);
		CLASS_FIELD(Tessellator*, tess, 0xC0);
	public:

		JpegCommentWriter(class Tessellator* tess, MaterialPtr* mat) { material = *mat; this->tess = tess; }

		void _drawImage(ScreenContext* scn, cg::ImageBuffer* image);
	};
}