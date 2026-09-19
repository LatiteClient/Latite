#include "pch.h"
#include "SetScorePacket.h"

#include <type_traits>

std::wstring SDK::SetScorePacket::serialize() const {
    nlohmann::json serialized;

    const auto packetType = !scoreInfo.empty() && std::holds_alternative<RemoveScore>(scoreInfo.front())
                                ? PacketType::Remove
                                : PacketType::Change;
    serialized["type"] = static_cast<int>(packetType);

    for (const auto& info : scoreInfo) {
        nlohmann::json score;

        std::visit(
            [&score](const auto& entry) {
                using Entry = std::remove_cvref_t<decltype(entry)>;

                score["scoreboardId"] = entry.scoreboardId.rawId;

                if constexpr (std::is_same_v<Entry, RemoveScore>) {
                    score["objectiveName"] = entry.objectiveName.value_or("");
                    score["scoreValue"] = 0;
                    score["identityType"] = static_cast<uint8_t>(IdentityType::Invalid);
                    score["playerId"] = 0;
                    score["entityId"] = 0;
                    score["fakePlayerName"] = "";
                } else {
                    score["objectiveName"] = entry.objectiveName;
                    score["scoreValue"] = entry.scoreValue;

                    if constexpr (std::is_same_v<Entry, ChangePlayerScore>) {
                        score["identityType"] = static_cast<uint8_t>(IdentityType::Player);
                        score["playerId"] = entry.playerId;
                        score["entityId"] = 0;
                        score["fakePlayerName"] = "";
                    } else if constexpr (std::is_same_v<Entry, ChangeEntityScore>) {
                        score["identityType"] = static_cast<uint8_t>(IdentityType::Entity);
                        score["playerId"] = 0;
                        score["entityId"] = entry.entityId;
                        score["fakePlayerName"] = "";
                    } else {
                        score["identityType"] = static_cast<uint8_t>(IdentityType::FakePlayer);
                        score["playerId"] = 0;
                        score["entityId"] = 0;
                        score["fakePlayerName"] = entry.fakePlayerName;
                    }
                }
            },
            info);

        serialized["scoreInfo"].push_back(score);
    }

    return util::StrToWStr(serialized.dump());
}
