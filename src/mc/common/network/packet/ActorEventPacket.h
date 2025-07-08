#pragma once
#include "../Packet.h"

namespace SDK {
	enum class ActorEventID : unsigned char {
		HURT_ANIMATION = 2,
		DEATH_ANIMATION,
		ARM_SWING,
		TAME_FAIL = 5,
		TAME_SUCCESS = 7,
		SHAKE_WET = 8,
		USE_ITEM = 9,
		EAT_BLOCK_ANIMATION = 10,
		FISH_HOOK_BUBBLE = 11,
		FISH_HOOK_POSITION = 12
		// .. and more i didnt bother adding
	};

	class ActorEventPacket : public Packet {
	public:
		int64_t runtimeID;
		ActorEventID eventID;
		int data;
	};
}