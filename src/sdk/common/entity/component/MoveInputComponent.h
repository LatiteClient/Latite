#pragma once
#include "sdk/util.h"

namespace SDK {
	// or MoveInputHandler in older versions
	struct MoveInputComponent {
	private:
		char pad[0x88];
		
	public:
		MVCLASS_FIELD(bool, sneak, 0x00, 0x4C, 0x4C);
		MVCLASS_FIELD(bool, jump, 0x06, 0x4B, 0x4B);
		MVCLASS_FIELD(bool, sprintKey, 0x07, 0x4C, 0x4C);
		MVCLASS_FIELD(bool, front, 0x2C, 0x0A, 0x5F, 0x5F);
		MVCLASS_FIELD(bool, back, 0x2D, 0x0B, 0x60, 0x60);
		MVCLASS_FIELD(bool, left, 0x2E, 0x0C, 0x61, 0x61);
		MVCLASS_FIELD(bool, right, 0x2F, 0x0D, 0x62, 0x62);
		MVCLASS_FIELD(bool, sprinting, 0x27, 0x6C, 0x6C);
	};
}
