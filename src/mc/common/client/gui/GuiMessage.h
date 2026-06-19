#pragma once

namespace SDK {
    struct GuiMessage {
        int type; // 0x0
        std::string message; // 0x8
        std::optional<std::string> unk; // 0x28
        std::string ttsMessage; // 0x50
        std::string username; // 0x70
        std::string fullString; // 0x90
        std::optional<std::string> filteredMessage; // 0xB0
        std::string xuid; // 0xD8
        bool forceVisible; // 0xF8
        bool ttsRequired; // 0xF9
        float duration; // 0xFC
        bool hasBeenSeen; // 0x100
        int64_t pad; // 0x108
    };
}
