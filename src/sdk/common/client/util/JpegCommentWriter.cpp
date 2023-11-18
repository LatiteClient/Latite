#include "pch.h"
#include "JpegCommentWriter.h"

void SDK::JpegCommentWriter::_drawImage(ScreenContext* scn, cg::ImageBuffer* image) {
	static auto func = util::FindSignature("48 8b c4 55 53 56 57 41 55 41 56 41 57 48 8d 6c 24 ? 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 44 0f 29 40 ? 44 0f 29 48 ? 44 0f 29 90 ? ? ? ? 44 0f 29 98 ? ? ? ? 44 0f 29 a0 ? ? ? ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 45 ? 4d 8b f0");

	reinterpret_cast<void(*)(JpegCommentWriter*, ScreenContext*, cg::ImageBuffer*)>(func)(this, scn, image);
}
