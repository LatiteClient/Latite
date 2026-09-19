#pragma once
#include "../Packet.h"

#include <optional>
#include <variant>

namespace SDK {

    class SetScorePacket : public Packet {
    private:
        enum class PacketType : uint8_t {
            Change = 0x0,
            Remove = 0x1
        };

        enum class IdentityType : uint8_t {
            Invalid = 0x0,
            Player = 0x1,
            Entity = 0x2,
            FakePlayer = 0x3
        };

        class IdentityDef;

        struct ScoreboardId {
            int64_t rawId;
            IdentityDef* identityDef;
        };

        struct RemoveScore {
            ScoreboardId scoreboardId;
            std::optional<std::string> objectiveName;
        };

        struct ChangePlayerScore {
            ScoreboardId scoreboardId;
            std::string objectiveName;
            int scoreValue;
            int64_t playerId;
        };

        struct ChangeEntityScore {
            ScoreboardId scoreboardId;
            std::string objectiveName;
            int scoreValue;
            int64_t entityId;
        };

        struct ChangeFakePlayerScore {
            ScoreboardId scoreboardId;
            std::string objectiveName;
            int scoreValue;
            std::string fakePlayerName;
        };

        using ScoreInfo = std::variant<RemoveScore, ChangePlayerScore, ChangeEntityScore, ChangeFakePlayerScore>;

    public:
        std::vector<ScoreInfo> scoreInfo;
        uint32_t serializationMode;

        std::wstring serialize() const;
    };
}
