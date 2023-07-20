#pragma once
#include "api/feature/Feature.h"
#include <variant>

struct BoolValue {
	bool value;

	BoolValue() { value = false; }
	BoolValue(bool b) : value(b) {}
	operator decltype(value)(){ return value; }
};

struct FloatValue {
	float value;

	FloatValue() { value = 0.f; }
	FloatValue(float f) : value(f) {}
	operator decltype(value)() { return value; }
};

struct IntValue {
	int value;

	IntValue() { value = 0; }
	IntValue(int i) : value(i) {}
	operator decltype(value)() { return value; }
};

struct KeyValue {
	int value;

	KeyValue() { value = 0; }
	KeyValue(int i) : value(i) {}
	KeyValue(char ch) : value((int)ch) {}
	operator decltype(value)(){ return value; }
};

class Setting : public Feature {
public:
	enum class Type {
		Bool,
		Int,
		Float,
		Key,
	};

	using Value = std::variant<BoolValue, FloatValue, IntValue, KeyValue>;

	Setting(std::string const& internalName, std::string const& displayName, std::string const& description, Type type) : settingName(internalName), displayName(displayName), description(description), type(type) {}

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
	std::string settingName, displayName, description;

};
