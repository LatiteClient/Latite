#pragma once
#include "util/LMath.h"

namespace SDK {
	class LevelRendererPlayer {
	private:
		MVCLASS_FIELD(SDK::MaterialPtr*, selectionBoxMaterial, 0xF50, 0x15A0, 0x1598);
	public:
		Vec3& getOrigin();
		float& getFovX();
		float& getFovY();

		class MaterialPtr* getSelectionBoxMaterial();
	};
}