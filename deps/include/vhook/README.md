# VTHook

Simple internal VTable hooker

Example:

```cpp
#include "VTHook/vtable_hook.h"

class Example {
    public:
        virtual int hi(bool t) {
            return t ? 129 : 123;
        }
}

typedef void(*hi_t)(bool);
hi_t original;
void hookHi(bool) {
    printf("hooking!\n");
    return original(true);
}

int main() {
    Example example;
    uintptr_t** VTExample = (uintptr_t**)example
    uintptr_t*& hiFunc = std::ref(VTExample[0]);
    
    vh::hook(reinterpret_cast<LPVOID*>(&hiFunc), hookHi, reinterpret_cast<LPVOID**>(&original));
											   // hoooking!
    printf("result: %i\n", example.hi(false)); // result: 129
    vh::unhook(reinterpret_cast<LPVOID*>(&hiFunc));
    
                                               
    printf("result: %i\n", example.hi(false)); // result: 123
}
```