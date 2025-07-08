#include "pch.h"
#include "ContainerScreenController.h"

void SDK::ContainerScreenController::_handleTakePlace(const std::string& viewName, int slot, bool b) {
	memory::callVirtual<int>(this, 59, viewName, slot, b);
}

void* SDK::ContainerScreenController::_getSelectedSlotInfo() {
	return memory::callVirtual<void*>(this, 63);
}
