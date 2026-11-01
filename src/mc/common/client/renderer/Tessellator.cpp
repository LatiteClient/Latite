#include "pch.h"
#include "Tessellator.h"

bool& SDK::Tessellator::isFormatFixed() {
    return hat::member_at<bool>(this, 0);
}

std::array<bool, 15>& SDK::Tessellator::getEnabledFields() {
    return hat::member_at<std::array<bool, 15>>(this, 0x158);
}

std::optional<uint32_t>& SDK::Tessellator::getNextColor() {
    return hat::member_at<std::optional<uint32_t>>(this, 0x1B0);
}

bool& SDK::Tessellator::isNoColor() {
    return hat::member_at<bool>(this, 0x254);
}

void SDK::Tessellator::vertex(float x, float y, float z) {
    reinterpret_cast<void (*)(Tessellator*, float, float, float)>(Signatures::Tessellator_vertex.result)(this, x, y, z);
}

void SDK::Tessellator::begin(Primitive fmt, int numVertices) {
    reinterpret_cast<void (*)(Tessellator*, int, Primitive, int, bool)>(Signatures::Tessellator_begin.result)(
        this, 0, fmt, numVertices, false); // buildFaceData
}

void SDK::Tessellator::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (isNoColor())
        return;

    getNextColor() = a << 24 | b << 16 | g << 8 | r;

    if (!isFormatFixed())
        this->getEnabledFields()[1] = true;
}

void SDK::Tessellator::color(float r, float g, float b, float a) {
    color(static_cast<uint8_t>(r * 255.f), static_cast<uint8_t>(g * 255.f), static_cast<uint8_t>(b * 255.f), static_cast<uint8_t>(a * 255.f));
}

void SDK::Tessellator::color(Color const& col) {
    color(col.r, col.g, col.b, col.a);
}
