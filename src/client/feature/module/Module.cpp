#include "Module.h"

void Module::loadConfig(SettingGroup& resolvedGroup) {
	resolvedGroup.forEach([&](std::shared_ptr<Setting> set) {
		this->settings->forEach([&](std::shared_ptr<Setting> modSet) {
			if (modSet->name() == set->name()) {
				std::visit([&](auto&& obj) {
					*modSet->value = obj;
					modSet->update();
					}, set->resolvedValue);
			}
			});
		});
	afterLoadConfig();
}

std::shared_ptr<Setting> Module::addSetting(std::string const& internalName, std::string const& disp, std::string const& desc, ValueType& val, Setting::Condition condition) {
	auto set = std::make_shared<Setting>(internalName, disp, desc, condition);
	set->value = &val;
	settings->addSetting(set);
	return set;
}

std::shared_ptr<Setting> Module::addEnumSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, EnumData& dat, Setting::Condition condition) {
	auto set = std::make_shared<Setting>(internalName, displayName, desc, condition);
	set->enumData = &dat;
	set->value = dat.getValue();
	settings->addSetting(set);
	return set;
}

std::shared_ptr<Setting> Module::addSliderSetting(std::string const& internalName, std::string const& displayName, std::string const& desc, ValueType& val, ValueType min, ValueType max, ValueType interval, Setting::Condition condition) {

	auto set = std::make_shared<Setting>(internalName, displayName, desc, condition);
	set->value = &val;
	set->min = min;
	set->max = max;
	set->interval = interval;
	settings->addSetting(set);
	return set;
}
