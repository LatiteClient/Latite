#include "Config.h"
#include <fstream>
#include "json/json.hpp"

using nlohmann::json;

Config::Config(std::filesystem::path path) : path(path) {
}

std::optional<errno_t> Config::load() {
	std::ifstream ifs;
	ifs.open(path);
	if (ifs.fail()) {
		if (errno != ENOENT) {
			return errno;
		}
		std::ofstream ofs;
		ofs.open(path);
		ofs << "{}";
		ofs.close();
		return load();
	}

	json obj = {};

	try {
		obj = json::parse(ifs);
	}
	catch (json::parse_error&) {
		return 0;
	}

	if (obj["settings"].is_array()) {
		for (auto& objc : obj["settings"]) {
			addGroup(objc);
		}
	}
	return std::nullopt;
}

void Config::addGroup(nlohmann::json obj) {
	if (obj["name"].is_string()) {
		auto group = std::make_shared<SettingGroup>(obj["name"].get<std::string>());
		// ...
		if (obj["settings"].is_array()) {
			for (auto& set : obj["settings"]) {
				if (set["type"].is_number()) {
					addSetting(*group.get(), set);
				}
			}
		}
		out.push_back(group);
	}
}

void Config::addSetting(SettingGroup& group, nlohmann::json& obj) {
	auto set = std::make_shared<Setting>(obj["name"].get<std::string>(), "", "");
	auto jVal = obj["value"];
	auto type = obj["type"].get<int>();

	// TODO: I think I need to do some metaprogramming magic instead of a spooky switch statement

	
	switch ((Setting::Type)type) {
	case Setting::Type::Bool:
		set->resolvedValue = BoolValue(jVal);
		break;
	case Setting::Type::Key:
		set->resolvedValue = KeyValue(jVal);
		break;
	case Setting::Type::Int:
		set->resolvedValue = IntValue(jVal);
		break;
	case Setting::Type::Float:
		set->resolvedValue = FloatValue(jVal);
		break;
	case Setting::Type::Color:
		set->resolvedValue = ColorValue(jVal);
		break;
	case Setting::Type::Vec2:
		set->resolvedValue = Vec2Value(jVal);
		break;
	case Setting::Type::Enum:
		set->resolvedValue = EnumValue(jVal);
		break;
	case Setting::Type::Text:
		set->resolvedValue = TextValue(jVal);
		break;
	default:
		return;
	}
	group.addSetting(set);
}

std::optional<errno_t> Config::save(std::vector<SettingGroup*> list) {
	json jout;
	jout["settings"] = json::array();
	for (auto& grp : list) {
		json js = json::object();
		saveGroup(*grp, js);
		jout["settings"].push_back(js);
	}

	std::ofstream ofs;
	ofs.open(this->path);
	if (!ofs.fail()) {
		ofs << std::setw(4) << jout;
		return std::nullopt;
	}
	return errno;
}

void Config::saveGroup(SettingGroup& group, json& j) {

	j["name"] = group.name();
	j["settings"] = json::array();
	group.forEach([&](std::shared_ptr<Setting> set) {
		json jset;
		saveSetting(set, jset);
		j["settings"].push_back(jset);
		});
}

void Config::saveSetting(std::shared_ptr<Setting> set, nlohmann::json& jout) {
	jout["name"] = set->name();
	jout["type"] = (*set->value).index();
	auto val = *set->value;

	std::visit([&](auto&& obj) {
		nlohmann::json objc = json::object();
		obj.store(objc);
		jout["value"] = objc;
		}, *set->value);
}

std::vector<std::shared_ptr<SettingGroup>> Config::getOutput() noexcept {
	return out;
}
