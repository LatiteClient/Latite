#pragma once
#include "util/Util.h"

namespace SDK {

    // I'm not sure about this
    enum class Primitive {
        Quad = 1,
        TriangleFan,
        Trianglestrip,
        LineList
    };

    class Tessellator {
    public:
        bool& isFormatFixed();
        std::array<bool, 15>& getEnabledFields();
        std::optional<uint32_t>& getNextColor();
        bool& isNoColor();

        void vertex(float x, float y, float z = 0.f);
        void begin(Primitive fmt, int numVertices);
        void color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        void color(float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);
        void color(Color const& col);
    };
}
