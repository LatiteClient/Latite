#include "pch.h"
#include "SetScorePacket.h"

std::wstring SDK::SetScorePacket::serialize() const {
    nlohmann::json serialized;

    serialized["type"] = static_cast<int>(type);

    for (const auto& info : scoreInfo) {
        nlohmann::json score;
        score["scoreboardId"] = info.scoreboardId.rawId;
        score["objectiveName"] = info.objectiveName;
        score["scoreValue"] = info.scoreValue;
        score["identityType"] = static_cast<unsigned char>(info.identityType);
        score["playerId"] = info.playerId;
        score["entityId"] = info.entityId;
        score["fakePlayerName"] = info.fakePlayerName;

        serialized["scoreInfo"].push_back(score);
    }

    return util::StrToWStr(serialized.dump());
}