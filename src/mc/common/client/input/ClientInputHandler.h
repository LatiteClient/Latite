#pragma once
#include "mc/Util.h"
#include "RemappingLayout.h"

namespace SDK {
    class ClientInputMappingFactory {
    public:
        // std::weak_ptr<KeyboardLayout>::_Ptr; the control block follows at 0x118.
        CLASS_FIELD(KeyboardLayout*, defaultKeyboardLayout, 0x110);
    };

    class ClientInputHandler {
    public:
        CLASS_FIELD(ClientInputMappingFactory*, mappingFactory, 0x48); // ClientInputHandler::getMappingFactory
    };
}
