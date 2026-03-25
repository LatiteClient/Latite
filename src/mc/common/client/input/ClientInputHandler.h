#pragma once
#include "mc/Util.h"
#include "RemappingLayout.h"

namespace SDK {
	class ClientInputMappingFactory {
	public:
		CLASS_FIELD(KeyboardLayout*, defaultKeyboardLayout, 0x110);
	};

	class ClientInputHandler {
	public:
		CLASS_FIELD(ClientInputMappingFactory*, mappingFactory, 0x30); // ClientInputHandler::getMappingFactory
	};
}