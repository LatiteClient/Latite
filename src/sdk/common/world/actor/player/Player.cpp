#include "Player.h"
#include "util/Util.h"
#include "api/memory/memory.h"
#include "sdk/version.h"

void sdk::Player::displayClientMessage(std::string const& message) {
	int index = 0;
	switch (sdk::internalVers) {
	case V1_18_12:
		index = 0x184;
		break;
	case V1_19_41:
		index = 0x18C;
		break;
	case V1_19_51:
		index = 0x18A;
		break;
	default:
		index = 0x165;
		break;
	}
	memory::callVirtual<void>(this, index, message);
}
