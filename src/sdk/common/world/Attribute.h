#pragma once
#include "sdk/Util.h"
#include "sdk/deps/Core/StringUtils.h"
#include "util/FNV32.h"

namespace SDK {
	class AttributeInstance {
	public:
		// AttributeInstance::getCurrentValue
		CLASS_FIELD(float, value, 0x84);
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
		inline static Attribute Hunger = Attribute(1, "minecraft:hunger");
		inline static Attribute Saturation = Attribute(2, "minecraft:saturation");
		inline static Attribute Health = Attribute(6, "minecraft:health");
	};
}