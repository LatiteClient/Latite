#pragma once
#include "mc/Util.h"

namespace SDK {
    class FontRepository {
    public:
        CLASS_FIELD(std::vector<std::shared_ptr<class Font>>, fontList, 0x40);

        const std::unordered_map<std::string, uint64_t>& getFontToIdMap() {
            return hat::member_at<std::unordered_map<std::string, uint64_t>>(this, 0x58);
        }

        class Font* getMinecraftFont() {
            static uint64_t id = UINT64_MAX;

            if (id == UINT64_MAX) id = this->getFontToIdMap().at("DefaultFont");

            return fontList[id].get();
        }

        class Font* getSmoothFont() { return fontList[7].get(); }
    };
}
