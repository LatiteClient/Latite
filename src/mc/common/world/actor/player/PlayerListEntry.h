#pragma once
#include "mc/Util.h"
#include "mc/common/world/actor/player/SerializedSkinRef.h"
#include <string>

namespace SDK {
	class PlayerListEntry {
		char pad[0xA8];

	public:
		CLASS_FIELD(std::string, name, 0x18);
		CLASS_FIELD(SerializedSkinRef, skin, 0x80);
	};
}
