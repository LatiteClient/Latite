#pragma once
#include <cstdint>
#include "util/ChakraUtil.h"
#include "sdk/common/world/actor/Actor.h"

class JsEntity {
public:
	enum class AccessLevel {
		Restricted,
		Operator,
		LocalPlayer
	};

	int64_t runtimeId;
	AccessLevel level;

	bool validate();
	SDK::Actor* getEntity();

	JsEntity(int64_t runtimeId, AccessLevel level = AccessLevel::Restricted);
};