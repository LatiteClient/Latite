#pragma once
#include "client/event/Event.h"
#include "util/Crypto.h"
#include "util/LMath.h"

class TurnDeltaEvent : public Event {
public:
    static const uint32_t hash = TOHASH(TurnDeltaEvent);

    explicit TurnDeltaEvent(Vec2& delta)
        : delta(delta) {}

    [[nodiscard]] Vec2& getDelta() { return delta; }
    void setDelta(Vec2 const& value) { delta = value; }

private:
    Vec2& delta;
};
