#include "pch.h"
#include "SetScorePacket.h"
#include "sdk/Util.h"

namespace SDK {
    // to_json for ActorUniqueID
    void to_json(nlohmann::json& j, const ActorUniqueID& actorUniqueId) {
        j = { {"id", actorUniqueId.id} };
    }

    // to_json for PlayerScoreboardId
    void to_json(nlohmann::json& j, const PlayerScoreboardId& playerId) {
        j = { {"mActorUniqueId", playerId.mActorUniqueId} };
    }

    // to_json for IdentityDefinition::Type
    void to_json(nlohmann::json& j, const IdentityDefinition::Type& type) {
        j = static_cast<unsigned char>(type); // Serialize as an unsigned char (underlying type)
    }

    // to_json for IdentityDefinition
    void to_json(nlohmann::json& j, const IdentityDefinition& identityDef) {
        j = nlohmann::json{
            {"mScoreboardId", identityDef.mScoreboardId.mRawId},  // Serialize just the mRawId from ScoreboardId
            {"mIsHiddenFakePlayer", identityDef.mIsHiddenFakePlayer},
            {"mPlayerId", identityDef.mPlayerId},
            {"mEntityId", identityDef.mEntityId},
            {"mPlayerName", identityDef.mPlayerName},
            {"mIdentityType", identityDef.mIdentityType}
        };
    }

    // to_json for ScoreboardId
    void to_json(nlohmann::json& j, const ScoreboardId& scoreboardId) {
        j = nlohmann::json{
            {"mRawId", scoreboardId.mRawId}
        };

        // Only add mIdentityDef if it's not nullptr, serialize just the raw ID to break circular reference
        if (scoreboardId.mIdentityDef) {
            j["mIdentityDef"] = scoreboardId.mIdentityDef->mScoreboardId.mRawId;
        }
    }

    // to_json for ScorePacketInfo
    void to_json(nlohmann::json& j, const ScorePacketInfo& scorePacketInfo) {
        j = nlohmann::json{
            {"mScoreboardId", scorePacketInfo.mScoreboardId},
            {"mObjectiveName", scorePacketInfo.mObjectiveName},
            {"mScoreValue", scorePacketInfo.mScoreValue},
            {"mIdentityType", scorePacketInfo.mIdentityType},
            {"mPlayerId", scorePacketInfo.mPlayerId},
            {"mEntityId", scorePacketInfo.mEntityId},
            {"mFakePlayerName", scorePacketInfo.mFakePlayerName}
        };
    }

    // to_json for SetScorePacket
    void to_json(nlohmann::json& j, const SetScorePacket& pkt) {
        j = nlohmann::json{
            {"mType", static_cast<int>(pkt.mType)}, // Serialize ScorePacketType as integer
            {"mScoreInfo", pkt.mScoreInfo}           // Serialize each ScorePacketInfo in the vector
        };
    }

    // Implementation of serializeSetScorePacket
    nlohmann::json serializeSetScorePacket(const std::shared_ptr<SetScorePacket>& pkt) {
        nlohmann::json j;
        to_json(j, *pkt); // Use the to_json function to serialize SetScorePacket
        return j;
    }
}