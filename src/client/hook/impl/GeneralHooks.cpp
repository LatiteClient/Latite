#include "GeneralHooks.h"

void GeneralHooks::Level_tick(sdk::Level* level)
{
}

void GeneralHooks::ChatScreenController_sendChatMessage(void* controller, std::string message)
{
}

GeneralHooks::GeneralHooks() : HookGroup("General") {

}
