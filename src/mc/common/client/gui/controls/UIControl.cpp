#include "pch.h"
#include "UIControl.h"

#include "mc/Addresses.h"

void SDK::UIControl::updatePos() {
    Vec2 add{};

    if (this->parent != nullptr) {
        if (this->parent->flags & 1)
            this->parent->updatePos();

        add = this->parent->position;
    }

    this->position = this->parentRelativePosition + add;
    this->flags &= ~1;
}
