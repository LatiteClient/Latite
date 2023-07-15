#pragma once
#include "api/feature/Feature.h"

class Setting : public Feature {
public:
	enum class Type {
		Bool,
		Int,
		Float,
	};

	std::string desc() override { return description; }
	std::string name() override { return settingName; }

protected:
	std::string settingName, description;

};
