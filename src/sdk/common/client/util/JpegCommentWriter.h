#pragma once
namespace SDK {
	namespace cg {
		class ImageBuffer;
	}
	
	class JpegCommentWriter {
		[[maybe_unused]] char pad[0xA8];
	public:

		// ui_texture_and_color
		class MaterialPtr* material;

		JpegCommentWriter(MaterialPtr* mat) : material(mat) {}

		void _drawImage(ScreenContext* scn, cg::ImageBuffer* image);
	};
}