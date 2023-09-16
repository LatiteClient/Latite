#pragma once
#include "api/feature/Feature.h"
#include "json/json.hpp"
#include <variant>

struct NullValue {
	NullValue() = default;

	int getInt() {
		return 0;
	}
};

struct BoolValue {
	bool value;

	BoolValue() { value = false; }
	BoolValue(bool b) : value(b) {}
	BoolValue(nlohmann::json& js) : value(js.get<bool>()) {}
	operator decltype(value)(){ return value; }

	void store(nlohmann::json& jout) {
		jout = value;
	}

	int getInt() {
		return static_cast<int>(value);
	}
};

struct FloatValue {
	float value;

	FloatValue() { value = 0.f; }
	FloatValue(float f) : value(f) {}
	FloatValue(nlohmann::json& js) : value(js.get<float>()) {}
	operator decltype(value)() { return value; }

	void store(nlohmann::json& jout) {
		jout = value;
	}

	int getInt() {
		return static_cast<int>(value);
	}
};

struct Vec2Value {
	float x, y;

	Vec2Value() { x = 0.f; y = 0.f; }
	Vec2Value(float x, float y) : x(x), y(y) {}
	Vec2Value(nlohmann::json& js) {
		x = js["x"].get<float>();
		y = js["y"].get<float>();
	}

	void store(nlohmann::json& jout) {
		jout["x"] = x;
		jout["y"] = y;
	}

	int getInt() {
		return 0;
	}
};

struct IntValue {
	int value;

	IntValue() { value = 0; }
	IntValue(int i) : value(i) {}
	IntValue(nlohmann::json& js) : value(js.get<int>()) {}

	void store(nlohmann::json& jout) {
		jout = value;
	}

	int getInt() {
		return static_cast<int>(value);
	}
};

struct KeyValue {
	int value;

	KeyValue() { value = 0; }
	KeyValue(int i) : value(i) {}
	KeyValue(char ch) : value((int)ch) {}
	KeyValue(nlohmann::json& js) : value(js.get<int>()) {}

	operator int() {
		return value;
	}

	void store(nlohmann::json& jout) {
		jout = value;
	}

	int getInt() {
		return static_cast<int>(value);
	}
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

	void get(nlohmann::json& js) {
		r = js["r"].get<float>();
		g = js["g"].get<float>();
		b = js["b"].get<float>();
		a = js["a"].get<float>();
	}

	int getInt() {
		// maybe use bit_cast if needed
		return 0;
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

	ColorValue(nlohmann::json& js) {
		color1.get(js["color1"]);
		isRGB = js["isRGB"].get<bool>();
		isChroma = js["isChroma"].get<bool>();
		if (isChroma) {
			numColors = js["numColors"].get<int>();
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

	int getInt() {
		return 0;
	}
};

struct TextValue {
	std::wstring str;

	TextValue(std::wstring const& str) : str(str) {};

	TextValue(nlohmann::json& js) {
		str = js.get<std::wstring>();
	}

	void store(nlohmann::json& jout) {
		jout = str;
	}

	int getInt() {
		return 0;
	}
};

struct EnumValue {
	int val = 0;

	EnumValue(int val) : val(val) {};

	EnumValue(nlohmann::json& js) {
		val = js.get<int>();
	}

	void store(nlohmann::json& jout) {
		jout = val;
	}

	int getInt() {
		return val;
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
	EnumValue,
	TextValue>;

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
	struct Condition final {
		std::string settingName = "";
		std::vector<int> values = {};

		enum Type {
			NONE,
			EQUALS,
			NOT,
		} type;

		Condition(const char* name /*shut up c++*/) : type(EQUALS), settingName(name), values({1}) {}
		Condition(std::string const& name = "", Type type = NONE, std::vector<int> values = {}) : type(type), settingName(name), values(std::move(values)) {}
		~Condition() = default;
	};

	enum class Type : size_t {
		Bool,
		Float,
		Int,
		Key,
		Color,
		Vec2,
		Enum,
		Text
	};

	Setting(std::string const& internalName, std::string const& displayName, std::string const& description, Condition condition = Condition()) : settingName(internalName), displayName(displayName), description(description), condition(std::move(condition)) {}

	[[nodiscard]] bool shouldRender(class SettingGroup& group);

	std::string desc() override { return description; }
	std::string name() override { return settingName; }

	std::string getDisplayName() { return displayName; }

	std::optional<std::function<void(Setting&)>> callback;

	virtual void update() {
		if (callback) callback.value()(*this);
	}

	EnumData* enumData = nullptr;
	ValueType* value = nullptr;

	ValueType resolvedValue;
	ValueType defaultValue;
	ValueType interval;
	ValueType min;
	ValueType max;

	Condition condition;

	bool visible = true;

	struct {
		bool init = false;
		float col[4] = { 0.f, 0.f, 0.f, 1.f };
	} rendererInfo;
protected:
	std::string settingName, displayName, description;

};

inline Setting::Condition operator"" _istrue(char const* s, size_t size) {
	return Setting::Condition(std::string(s, size), Setting::Condition::EQUALS, { 1 });
}

inline Setting::Condition operator"" _isfalse(char const* s, size_t size) {
	return Setting::Condition(std::string(s, size), Setting::Condition::EQUALS, { 0 });
}
