#pragma once
#include "sdk/Util.h"
#include "sdk/deps/Core/StringUtils.h"
#include "util/FNV32.h"

namespace SDK {
	class AttributeInstance {
	public:
		// AttributeInstance::getCurrentValue
		MVCLASS_FIELD(float, value, 0x7C, 0x84, 0x84, 0x84, 0x84, 0x84, 0x88, 0x84, 0x84, 0x84);
	};

	class Attribute {
	public:
		int8_t mRedefinitionNode = 0;
		bool mSyncable = true;
		short unk = 0;
		int mIDValue;
		HashedString mName;

		Attribute(int id, std::string const& name) : mIDValue(id), mName(name) {}
	};

	class Attributes {
	public:
		inline static Attribute Hunger = Attribute(SDK::mvGetOffset<1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1>(), "minecraft:hunger");
		inline static Attribute Saturation = Attribute(SDK::mvGetOffset<2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2>(), "minecraft:saturation");
		inline static Attribute Health = Attribute(SDK::mvGetOffset<7, 6, 1, 1, 1, 7, 7, 7, 7, 7, 6, 6, 6>(), "minecraft:health");
	};
}