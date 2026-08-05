#pragma once
#include "mc/Util.h"

#include <string>

namespace SDK::Social {
    class ThirdPartyInfo {
    public:
        CLASS_FIELD(std::string, creatorId, 0x40);
        CLASS_FIELD(std::string, creatorName, 0x60);
        CLASS_FIELD(std::string, storagePageId, 0x80);
        CLASS_FIELD(bool, requireXboxLive, 0xA0);
        CLASS_FIELD(std::string, experienceId, 0xA8);
    };

    class GameConnectionInfo {
    public:
        CLASS_FIELD(std::string, hostIpAddress, 0x8);
        CLASS_FIELD(std::string, unresolvedUrl, 0x28);
        CLASS_FIELD(std::string, serverRegion, 0x48);
        CLASS_FIELD(int, serviceQuality, 0x88);
        CLASS_FIELD(int, port, 0x8C);
        CLASS_FIELD(ThirdPartyInfo, thirdPartyServerInfo, 0xD0);
    };
}
