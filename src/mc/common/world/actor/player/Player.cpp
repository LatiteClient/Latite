#include "pch.h"
#include "Player.h"
#include "util/Util.h"
#include "util/memory.h"
#include "mc/Version.h"
#include "mc/Addresses.h"

void SDK::Player::displayClientMessage(std::string const& message) {
	std::optional<std::string> opt = {};
	memory::callVirtual<void>(this, 0xC6, message, opt);
	return;
}

SDK::MoveInputComponent* SDK::Player::getMoveInputComponent() {
	using try_get_t = SDK::MoveInputComponent* (__fastcall*)(uintptr_t a1, uint32_t* a2);
	static auto func = reinterpret_cast<try_get_t>(Signatures::Components::moveInputComponent.result);
	return func(entityContext.getBasicRegistry(), &entityContext.getId());
}

std::string SDK::Player::getXUID() {
	return memory::callVirtual<std::string>(this, 0xE7);
}
