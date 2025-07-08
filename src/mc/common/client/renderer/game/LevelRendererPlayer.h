#pragma once
#include "mc/common/client/renderer/MaterialPtr.h"
#include "util/LMath.h"

namespace SDK {
	class LevelRendererPlayer {
	private:
		//MVCLASS_FIELD(class MaterialPtr*, selectionOverlayMaterial, 0xF60, 0x2F, 0xF08, 0xEF0, 0xEF0, 0x14E0, 0x1538);
		//MVCLASS_FIELD(class MaterialPtr*, selectionBoxMaterial, 0xFC0, 0x2F0, 0x2F0, 0xF50, 0xF50, 0x15A0, 0x1598);
		//CLASS_FIELD(class MaterialPtr*, selectionBoxMaterial, 0x2D8);
	public:

		Vec3& getOrigin();
		float& getFovX();
		float& getFovY();

		/*class MaterialPtr* getSelectionBoxMaterial() { return selectionBoxMaterial; };
		class MaterialPtr* getSelectionOverlayMaterial() { return selectionOverlayMaterial; };*/
	};
}