#pragma once
#include "client/event/Eventing.h"
#include "client/event/events/KeyUpdateEvent.h"
#include "client/input/ControllerInput.h"

class Keyboard final : public Listener {
public:
    Keyboard(int* gameKeyMap);
    Keyboard(Keyboard&) = delete;
    Keyboard(Keyboard&&) = delete;

    ~Keyboard() = default;

    int* keyMap;
    int keyMapAdjusted[0x100] = {};
    int keyMapOld[0x100] = {};
    BYTE winKeyMap[0x100] = {};
    bool controllerButtonMap[controller_input::BUTTON_COUNT] = {};

    void findTextInput();
    bool isKeyDown(int vKey);
    int getMappedKey(std::string const& name);
    void setControllerButtonState(int key, bool isDown);

    void onKey(Event& ev);

private:
    void onChar(wchar_t ch, bool isChar = true);
};
