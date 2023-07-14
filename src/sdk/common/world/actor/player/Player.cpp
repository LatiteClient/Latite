#include "Player.h"
#include "util/util.h"
#include "api/memory/memory.h"

void sdk::Player::displayClientMessage(std::string const& message)
{
	memory::callVirtual<void>(this, 0x165, message);
}
