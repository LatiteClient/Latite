#pragma once

namespace SDK {
    class InteractionResult {
    public:
        bool mSuccess : 1;
        bool mSwing : 1;
    };

    static_assert(sizeof(InteractionResult) == 1);
}
