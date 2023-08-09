#include "Module.h"

void Module::loadConfig(SettingGroup& resolvedGroup) {
	resolvedGroup.forEach([&](std::shared_ptr<Setting> set) {
		this->settings->forEach([&](std::shared_ptr<Setting> modSet) {
			if (modSet->name() == set->name()) {
				switch ((Setting::Type)set->resolvedValue.index()) {
				case Setting::Type::Bool:
					*modSet->value = std::get<BoolValue>(set->resolvedValue);
					break;
				case Setting::Type::Int:
					*modSet->value = std::get<IntValue>(set->resolvedValue);
					break;
				case Setting::Type::Float:
					*modSet->value = std::get<FloatValue>(set->resolvedValue);
					break;
				case Setting::Type::Key:
					*modSet->value = std::get<KeyValue>(set->resolvedValue);
					break;
				case Setting::Type::Color:
					*modSet->value = std::get<ColorValue>(set->resolvedValue);
					break;
				case Setting::Type::Vec2: 
					*modSet->value = std::get<Vec2Value>(set->resolvedValue);
					break;
				case Setting::Type::Enum:
					*modSet->value = std::get<EnumValue>(set->resolvedValue);
					break;
				default:
					throw std::runtime_error("Setting not implemented");
					break;
				}
			}
			});
		});
	afterLoadConfig();
}

void Module::addSetting(std::string const& internalName, std::string const& disp, std::string const& desc, ValueType& val) {
	auto set = std::make_shared<Setting>(internalName, disp, desc);
	set->value = &val;
	settings->addSetting(set);
}

void Module::addEnumSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, EnumData& dat) {
	auto set = std::make_shared<Setting>(internalName, displayName, desc);
	set->enumData = &dat;
	set->value = dat.getValue();
	settings->addSetting(set);
}

void Module::addSliderSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval) {

	auto set = std::make_shared<Setting>(internalName, displayName, desc);
	set->value = &val;
	set->min = min;
	set->max = max;
	set->interval = interval;
	settings->addSetting(set);
}
