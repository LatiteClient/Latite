#pragma once
#include "mc/Util.h"
#include "RemappingLayout.h"

namespace SDK {
	class ClientInputMappingFactory {
	public:
		CLASS_FIELD(KeyboardLayout*, defaultKeyboardLayout, 0xF0);
	};

	class ClientInputHandler {
	public:
		MVCLASS_FIELD(ClientInputMappingFactory*, mappingFactory, 0x30, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x20, 0x20, 0x20, 0x20, 0x20, 0x18, 0x18); // ClientInputHandler::getMappingFactory
	};
}