#include "pch.h"
#include "Player.h"
#include "util/Util.h"
#include "util/memory.h"
#include "mc/Version.h"
#include "mc/Addresses.h"

void SDK::Player::displayClientMessage(std::string const& message) {
	if (internalVers >= V1_21_20) {
		std::optional<std::string> opt = {};
		memory::callVirtual<void>(this, mvGetOffset<0xC8, 0xCD, 0xCE, 0xD2, 0xD2, 0x0, 0x0, 0x0, 0x0, 0x0>(), message, opt);
		return;
	}
	memory::callVirtual<void>(this, mvGetOffset<0xEF, 0x15E, 0x184, 0x18A>(), message);
}

SDK::MoveInputComponent* SDK::Player::getMoveInputComponent() {
	// tryGetMoveInputHandler
	if (SDK::internalVers == V1_19_51) {
		return memory::callVirtual<SDK::MoveInputComponent*>(this, 0x1B7);
	}

	if (SDK::internalVers == V1_19_41) {
		return memory::callVirtual<SDK::MoveInputComponent*>(this, 0x1B9);
	}

	if (SDK::internalVers == V1_18_12) {
		return memory::callVirtual<SDK::MoveInputComponent*>(this, 0x1B1);
	}

	using try_get_t = SDK::MoveInputComponent* (__fastcall*)(uintptr_t a1, uint32_t* a2);
	static auto func = reinterpret_cast<try_get_t>(Signatures::Components::moveInputComponent.result);
	return func(entityContext.getBasicRegistry(), &entityContext.getId());
}

std::string SDK::Player::getXUID() {
	return memory::callVirtual<std::string>(this, SDK::mvGetOffset<0xEC, 0xF1, 0xF2, 0xF3, 0xF3, 0xF6, 0x18C, 0x1B6, 0x1BC>());
}
