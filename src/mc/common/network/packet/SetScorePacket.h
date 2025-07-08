#pragma once
#include "../Packet.h"

namespace SDK {

	class SetScorePacket : public Packet {
	private:
		enum class PacketType : uint8_t {
			Change = 0x0,
			Remove = 0x1
		};

		enum class IdentityType : unsigned char {
			Invalid = 0x0,
			Player = 0x1,
			Entity = 0x2,
			FakePlayer = 0x3
		};

		enum class DefinitionType : unsigned char {
			Invalid = 0x0,
			Player = 0x1,
			Entity = 0x2,
			FakePlayer = 0x3
		};

		class IdentityDef;

		struct ScoreboardId {
			int64_t		 rawId;
			IdentityDef* identityDef;
		};

		class IdentityDef {
			ScoreboardId	scoreboardId;
			bool			isHiddenFakePlayer;
			int64_t			playerId;
			int64_t			entityId;
			std::string		playerName;
			DefinitionType  identityType;
		};

		struct ScoreInfo {
			ScoreboardId   scoreboardId;
			std::string	   objectiveName;
			int			   scoreValue;
			DefinitionType identityType;
			int64_t		   playerId;
			int64_t        entityId;
			std::string	   fakePlayerName;
		};

	public:
		PacketType  type;
		std::vector<ScoreInfo> scoreInfo;
		std::wstring serialize() const;
	};
}