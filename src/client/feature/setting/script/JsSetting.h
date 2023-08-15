#pragma once
#include "api/feature/setting/Setting.h"
#include "util/ChakraUtil.h"

class JsSetting : public Setting {
public:
	JsSetting(std::string const& internalName, std::string const& displayName, std::string const& description)
		: Setting(internalName, displayName, description) {
		this->value = &jsVal;
	}

	ValueType jsVal = BoolValue(false);
};