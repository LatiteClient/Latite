#pragma once
#include "client/feature/Feature.h"
#include <nlohmann/json.hpp>
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
	FloatValue(nlohmann::json& js) : value(js.is_number() ? js.get<float>() : 0) {}
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
		x = js["x"].is_number() ? js["x"].get<float>() : 0;
		y = js["y"].is_number() ? js["y"].get<float>() : 0;
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

	StoredColor getMainColor() const;

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
		if (js.is_string()) {
			str = util::StrToWStr(js.get<std::string>());
		}
		else {
			str = js.get<std::wstring>();
		}
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

struct SnapValue {
	enum Type : int {
		Normal,
		MCUI,
		Module
	};

	enum Pos : int {
		Right,
		Middle,
		Left
	};

	Type type = Normal;
	Pos position = Right;
	std::string mod = "";
	int index = 0;
	bool doSnapping = false;

	SnapValue(nlohmann::json& js) {
		if (js.contains("type")) {
			doSnapping = true;
			this->type = js["type"].get<SnapValue::Type>();
			if (this->type == SnapValue::Module) {
				this->mod = js["module"].get<std::string>();
			}
			this->index = js["idx"].get<int>();
			this->position = js["pos"].get<SnapValue::Pos>();
		}
	}

	SnapValue() = default;

	void store(nlohmann::json& j) {
		if (doSnapping) {
			j["type"] = type;
			if (type == SnapValue::Module) {
				j["module"] = mod;
			}
			j["idx"] = index;
			j["pos"] = position;
		}
	}

	void snap(SnapValue::Type type, SnapValue::Pos pos, int idx, std::string mod = "") {
		this->doSnapping = true;
		this->type = type;
		this->position = pos;
		this->mod = mod;
		this->index = idx;
	}

	int getInt() {
		return 0;
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
	TextValue,
	SnapValue>;

class EnumEntry /*: Feature*/ {
	std::wstring entryName;
	std::wstring entryDesc;
public:
	std::wstring name() { return entryName; }
	std::wstring desc() { return entryDesc; }

	EnumEntry(int key, std::wstring const& name, std::wstring const& desc = L"") : entryName(name), entryDesc(desc) {}
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

	[[nodiscard]] std::vector<EnumEntry>* getEntries() {
		return &entries;
	}

	[[nodiscard]] int getSelectedKey() {
		return std::get<EnumValue>(selectedIdx);
	}

	[[nodiscard]] std::wstring getSelectedName() {
		return entries[std::get<EnumValue>(selectedIdx)].name();
	}

	[[nodiscard]] std::wstring getSelectedDesc() {
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
		Text,
		Snap
	};

	Setting(std::string const& internalName, std::wstring const& displayName, std::wstring const& description, Condition condition = Condition()) : settingName(internalName), displayName(displayName), description(description), condition(std::move(condition)) {}

	[[nodiscard]] bool shouldRender(class SettingGroup& group);

	std::wstring desc() override { return description; }
	std::wstring getDisplayName() { return displayName; }
	std::string name() override { return settingName; }
	

	std::optional<std::function<void(Setting&)>> callback;
	std::optional<std::function<void(Setting&)>> userUpdateCallback;

	void update() {
		if (callback) callback.value()(*this);
	}

	void userUpdate() {
		if (userUpdateCallback) userUpdateCallback.value()(*this);
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
	std::string settingName;
	std::wstring displayName, description;

};

inline Setting::Condition operator"" _istrue(char const* s, size_t size) {
	return Setting::Condition(std::string(s, size), Setting::Condition::EQUALS, { 1 });
}

inline Setting::Condition operator"" _isfalse(char const* s, size_t size) {
	return Setting::Condition(std::string(s, size), Setting::Condition::EQUALS, { 0 });
}
