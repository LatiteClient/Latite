#include "Config.h"

#include "util/logger.h"

#include <fstream>
using nlohmann::json;

Config::Config(std::filesystem::path path) : path(path)
{
}

std::optional<errno_t> Config::load()
{
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
	catch (json::parse_error& e) {
		return 0;
	}

	if (obj["settings"].is_array()) {
		for (auto& obj : obj["settings"]) {
			addGroup(obj);
		}
	}
	return std::nullopt;
}

void Config::addGroup(nlohmann::json obj)
{
	if (obj["name"].is_string()) {
		auto group = std::make_shared<SettingGroup>(obj["name"].get<std::string>());
		// ...
		if (obj["settings"].is_array()) {
			for (auto& set : obj["settings"]) {
				if (set["type"].is_number()) {
					addSetting(group, set);
				}
			}
		}
		out.push_back(group);
	}
}

void Config::addSetting(std::shared_ptr<SettingGroup> group, nlohmann::json& obj)
{
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
	group->addSetting(set);
}

std::vector<std::shared_ptr<SettingGroup>> Config::getOutput()
{
	return out;
}
