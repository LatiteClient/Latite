#pragma once

class MouseReleaseEvent : public Event {
public:
    MouseReleaseEvent() = default;

    static const uint32_t hash = TOHASH(MouseReleaseEvent);
};
