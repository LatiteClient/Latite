#pragma once

#include <glm/mat4x4.hpp>

#include "api/eventing/Event.h"

class RenderItemRotateEvent : public Cancellable {
public:
    static const uint32_t hash = TOHASH(RenderItemRotateEvent);

    RenderItemRotateEvent(glm::mat4x4& matrix, float angle, float x, float y, float z) : matrix(&matrix), angle(angle), x(x), y(y), z(z) {}

    [[nodiscard]] glm::mat4x4* getMatrix() const {
        return matrix;
    }

    [[nodiscard]] float getAngle() const {
        return angle;
    }

    [[nodiscard]] float getX() const {
        return x;
    }

    [[nodiscard]] float getY() const {
        return y;
    }

    [[nodiscard]] float getZ() const {
        return z;
    }

protected:
    glm::mat4x4* matrix;
    float angle;
    float x;
    float y;
    float z;
};
