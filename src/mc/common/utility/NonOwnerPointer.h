#pragma once

#include <memory>

namespace SDK {
    struct NonOwnerPointerControlBlock {
        bool mIsValid;
    };

    template<typename T>
    class NonOwnerPointer {
    public:
        std::shared_ptr<NonOwnerPointerControlBlock> mControlBlock;
        T* mPointer = nullptr;

        [[nodiscard]] T* get() const noexcept { return mControlBlock && mControlBlock->mIsValid ? mPointer : nullptr; }

        [[nodiscard]] explicit operator bool() const noexcept { return get() != nullptr; }
    };

    static_assert(sizeof(NonOwnerPointer<void>) == 0x18);
}
