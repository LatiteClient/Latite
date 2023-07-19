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
	auto set = std::make_shared<Setting>(obj["name"].get<std::string>(), "", (Setting::Type)obj["type"].get<int>());
	auto jVal = obj["value"];
	switch (set->type) {
	case Setting::Type::Bool:
		set->resolvedValue = BoolValue(jVal.get<bool>());
		break;
	case Setting::Type::Int:
		set->resolvedValue = IntValue(jVal.get<int>());
		break;
	case Setting::Type::Float:
		set->resolvedValue = FloatValue(jVal.get<float>());
		break;
	default:
		return;
		break;
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
	jout["type"] = set->type;
	auto val = *set->value;
	switch (set->type) {
	case Setting::Type::Bool:
		jout["value"] = std::get<BoolValue>(val).value;
		break;
	case Setting::Type::Float:
		jout["value"] = std::get<FloatValue>(val).value;
		break;
	case Setting::Type::Int:
		jout["value"] = std::get<IntValue>(val).value;
		break;
	default:
		break;
	}
}

std::vector<std::shared_ptr<SettingGroup>> Config::getOutput() noexcept {
	return out;
}
