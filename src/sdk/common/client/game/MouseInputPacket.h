#pragma once

struct MouseInputPacket { // Made up
    int16_t x;
    int16_t y;

private:
    int pad;

public:
    int8_t type;
    int8_t state;

private:
    char pad2[0x8];
};
