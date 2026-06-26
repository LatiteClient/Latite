#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"

class ClickEvent : public Cancellable {
public:
    enum class ClickType : int {
        None = 0,
        Left = 1,
        Right = 2,
        Middle = 3,
        Wheel = 4,
    };

    static const uint32_t hash = TOHASH(ClickEvent);

    [[nodiscard]] int getMouseButton() const { return btn; }
    [[nodiscard]] ClickType getClickType() const { return static_cast<ClickType>(btn); }
    [[nodiscard]] int isDown() const { return downOrDelta; }
    [[nodiscard]] char getWheelDelta() const { return downOrDelta; }

    ClickEvent(int btn, char dod)
        : btn(btn)
        , downOrDelta(dod) {}

private:
    int btn;
    char downOrDelta;
};
