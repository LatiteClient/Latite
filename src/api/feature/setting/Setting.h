#pragma once
#include "api/feature/Feature.h"
#include <variant>

struct BoolValue {
	bool value;

	BoolValue() { value = false; }
	BoolValue(bool b) : value(b) {}
};

struct FloatValue {
	float value;

	FloatValue() { value = 0.f; }
	FloatValue(float f) : value(f) {}
};

struct IntValue {
	int value;

	IntValue() { value = 0; }
	IntValue(int i) : value(i) {}
};

class Setting : public Feature {
public:
	enum class Type {
		Bool,
		Int,
		Float,
	};

	using Value = std::variant<BoolValue, FloatValue, IntValue>;

	Setting(std::string const& name, std::string const& description, Type type) : settingName(name), description(description), type(type) {}

	std::string desc() override { return description; }
	std::string name() override { return settingName; }

	Value* value = nullptr;
	Value resolvedValue;
	Value defaultValue;
	Value interval;
	Value min;
	Value max;
	Type type;
protected:
	std::string settingName, description;

};
