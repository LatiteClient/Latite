#include "pch.h"
#include "SetScorePacket.h"
#include "sdk/Util.h"

std::string SDK::makeJson(const SetScorePacket& packet) {
    std::ostringstream oss;

    oss << "{";

    oss << "\"type\": \"" << (packet.mType == ScorePacketType::Change ? "Change" : "Remove") << "\",";

    oss << "\"scoreInfo\": [";

    for (size_t i = 0; i < packet.mScoreInfo.size(); ++i) {
        const auto& scoreInfo = packet.mScoreInfo[i];

        if (i > 0) oss << ",";

        oss << "{";

        oss << "\"scoreboardId\": {"
            << "\"rawId\": " << scoreInfo.mScoreboardId.mRawId << ",";

        if (scoreInfo.mScoreboardId.mIdentityDef) {
            const auto& identityDef = *scoreInfo.mScoreboardId.mIdentityDef;
            oss << "\"identityDef\": {"
                << "\"scoreboardId\": " << identityDef.mScoreboardId.mRawId << ","
                << "\"isHiddenFakePlayer\": " << (identityDef.mIsHiddenFakePlayer ? "true" : "false") << ","
                << "\"playerId\": " << identityDef.mPlayerId.mActorUniqueId << ","
                << "\"entityId\": " << identityDef.mEntityId.id << ","
                << "\"playerName\": \"" << identityDef.mPlayerName << "\","
                << "\"identityType\": \"" << (identityDef.mIdentityType == IdentityDefinition::Type::Player ? "Player" :
                    identityDef.mIdentityType == IdentityDefinition::Type::Entity ? "Entity" :
                    identityDef.mIdentityType == IdentityDefinition::Type::FakePlayer ? "FakePlayer" : "Invalid") << "\""
                << "},";
        }
        else {
            oss << "\"identityDef\": null,";
        }

        oss << "\"objectiveName\": \"" << scoreInfo.mObjectiveName << "\","
            << "\"scoreValue\": " << scoreInfo.mScoreValue << ","
            << "\"identityType\": \"" << (scoreInfo.mIdentityType == IdentityDefinition::Type::Player ? "Player" :
                scoreInfo.mIdentityType == IdentityDefinition::Type::Entity ? "Entity" :
                scoreInfo.mIdentityType == IdentityDefinition::Type::FakePlayer ? "FakePlayer" : "Invalid") << "\","
            << "\"playerId\": " << scoreInfo.mPlayerId.mActorUniqueId << ","
            << "\"entityId\": " << scoreInfo.mEntityId.id << ","
            << "\"fakePlayerName\": \"" << scoreInfo.mFakePlayerName << "\"";

        oss << "}";
    }

    oss << "]";
    oss << "}";

    return oss.str();
}