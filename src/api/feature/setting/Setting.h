#pragma once
#include "api/feature/Feature.h"
#include "json/json.hpp"
#include <variant>

struct NullValue {
	NullValue() = default;
};

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

struct Vec2Value {
	float x, y;

	Vec2Value() { x = 0.f; y = 0.f; }
	Vec2Value(float x, float y) : x(x), y(y) {}
	Vec2Value(nlohmann::json& js) {
		x = js["x"];
		y = js["y"];
	}

	void store(nlohmann::json& jout) {
		jout["x"] = x;
		jout["y"] = y;
	}
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

struct StoredColor {
	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 1.f;

	void store(nlohmann::json& jout) {
		jout["r"] = r;
		jout["g"] = g;
		jout["b"] = b;
		jout["a"] = a;
	}

	void get(nlohmann::json const& js) {
		r = js["r"];
		g = js["g"];
		b = js["b"];
		a = js["a"];
	}
};

struct ColorValue {
	StoredColor color1{};
	StoredColor color2{};
	StoredColor color3{};
	int numColors = 1;
	bool isRGB = false;
	bool isChroma = false;
	float chromaSpeed = 0.f;
	float chromaDirection = 180.f;

	ColorValue(float r, float g, float b, float a = 1.f) {
		color1 = { r, g, b, a };
	}

	ColorValue() {
		color1 = { 1.f, 1.f, 1.f, 1.f };
	}

	ColorValue(nlohmann::json const& js) {
		color1.get(js["color1"]);
		isRGB = js["isRGB"];
		isChroma = js["isChroma"];
		if (isChroma) {
			numColors = js["numColors"];
			if (numColors >= 2) color2.get(js["color2"]);
			if (numColors >= 3) color3.get(js["color3"]);
		}
	}

	void store(nlohmann::json& jout) {
		auto col1 = nlohmann::json::object();
		auto col2 = nlohmann::json::object();
		auto col3 = nlohmann::json::object();

		color1.store(col1);
		color2.store(col2);
		color3.store(col3);

		jout["color1"] = col1;
		jout["isRGB"] = isRGB;
		jout["isChroma"] = isChroma;
		if (isChroma) {
			jout["numColors"] = numColors;
			if (numColors >= 2) jout["color2"] = col2;
			if (numColors >= 3) jout["color3"] = col3;
			jout["chromaSpeed"] = chromaSpeed;
			jout["chromaDirection"] = chromaDirection;
		}
	}
};

struct TextValue {
	std::string str;

	TextValue(std::string const& str = "") : str(str) {};

	TextValue(nlohmann::json const& js) {
		str = js.get<std::string>();
	}

	void store(nlohmann::json& jout) {
		jout = str;
	}
};

struct EnumValue {
	int val = 0;

	EnumValue(int val) : val(val) {};

	EnumValue(nlohmann::json const& js) {
		val = js.get<int>();
	}

	void store(nlohmann::json& jout) {
		jout = val;
	}

	operator int() {
		return val;
	}
};

using ValueType = std::variant<
	BoolValue, 
	FloatValue, 
	IntValue, 
	KeyValue, 
	ColorValue, 
	Vec2Value, 
	EnumValue>;

class EnumEntry : public Feature {
	std::string entryName;
	std::string entryDesc;
public:
	std::string name() override { return entryName; }
	std::string desc() override { return entryDesc; }

	EnumEntry(int key, std::string const& name, std::string const& desc = "") : entryName(name), entryDesc(desc) {}
};

class EnumData {
	std::vector<EnumEntry> entries = {};
	ValueType selectedIdx = EnumValue(0);
public:
	void addEntry(EnumEntry const& ent) {
		entries.push_back(ent);
	}

	[[nodiscard]] ValueType* getValue() {
		return &selectedIdx;
	}

	[[nodiscard]] int getSelectedKey() {
		return std::get<EnumValue>(selectedIdx);
	}

	[[nodiscard]] std::string getSelectedName() {
		return entries[std::get<EnumValue>(selectedIdx)].name();
	}

	[[nodiscard]] std::string getSelectedDesc() {
		return entries[std::get<EnumValue>(selectedIdx)].desc();
	}

	void next() {
		if (++std::get<EnumValue>(selectedIdx).val >= entries.size()) std::get<EnumValue>(selectedIdx) = 0;
	}

	EnumData() = default;
};

class Setting : public Feature {
public:
	enum class Type : size_t {
		Bool,
		Float,
		Int,
		Key,
		Color,
		Vec2,
		Enum
	};

	Setting(std::string const& internalName, std::string const& displayName, std::string const& description) : settingName(internalName), displayName(displayName), description(description) {}

	std::string desc() override { return description; }
	std::string name() override { return settingName; }

	std::string getDisplayName() { return displayName; }

	EnumData* enumData = nullptr;
	ValueType* value = nullptr;

	ValueType resolvedValue;
	ValueType defaultValue;
	ValueType interval;
	ValueType min;
	ValueType max;

	bool visible = true;

	struct {
		bool init = false;
		float col[4] = { 0.f, 0.f, 0.f, 1.f };
	} rendererInfo;
protected:
	std::string settingName, displayName, description;

};
