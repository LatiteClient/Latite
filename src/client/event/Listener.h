#pragma once

#include <cstdint>

// Abstract class
class Listener {
public:
    Listener() = default;
    virtual ~Listener() = default;

    virtual bool shouldListen() { return true; }
    virtual bool shouldListenToEvent(uint32_t) { return true; }
};
