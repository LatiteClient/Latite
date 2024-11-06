#pragma once
#include "sdk/Util.h"
#include "../Packet.h"
#include "sdk/String.h"

namespace SDK {

	enum class ScorePacketType : uint8_t {
		Change = 0x0,
		Remove = 0x1
	};

	enum class IdentityType : unsigned char {
		Invalid    = 0x0,
		Player	   = 0x1,
		Entity	   = 0x2,
		FakePlayer = 0x3
	};

	struct ActorUniqueID {
		int64_t id{};
	};

	struct PlayerScoreboardId {
		int64_t mActorUniqueId;
	};

	class IdentityDefinition;

	struct ScoreboardId {
		int64_t				mRawId;
		IdentityDefinition* mIdentityDef;
	};

	class IdentityDefinition {
	public:
		enum class Type : unsigned char {
			Invalid = 0x0,
			Player = 0x1,
			Entity = 0x2,
			FakePlayer = 0x3,
		};

		ScoreboardId       mScoreboardId;
		bool               mIsHiddenFakePlayer;
		PlayerScoreboardId mPlayerId;
		ActorUniqueID      mEntityId;
		std::string        mPlayerName;
		Type               mIdentityType;
	};

	struct ScorePacketInfo {
		ScoreboardId			 mScoreboardId;
		std::string				 mObjectiveName;
		int						 mScoreValue;
		IdentityDefinition::Type mIdentityType;
		PlayerScoreboardId		 mPlayerId;
		ActorUniqueID            mEntityId;
		std::string				 mFakePlayerName;
	};

	class SetScorePacket : public Packet {
	public:
		ScorePacketType mType;
		std::vector<ScorePacketInfo> mScoreInfo;
	};

	std::string makeJson(const SetScorePacket& packet);
}