#pragma once
#include "client/feature/setting/Setting.h"
#include "util/ChakraUtil.h"

class JsSetting : public Setting {
public:
	JsSetting(std::string const& internalName, std::wstring const& displayName, std::wstring const& description)
		: Setting(internalName, displayName, description) {
		this->value = &jsVal;
	}

	ValueType jsVal = BoolValue(false);

	std::optional<EnumData> scriptEnumData;
};