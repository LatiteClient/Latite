#include "pch.h"

ClickMap* ClickMap::get() {
    return Signatures::Misc::clickMap.as_ptr<ClickMap>();
}
