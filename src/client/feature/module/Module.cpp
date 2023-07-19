#include "Module.h"

void Module::loadConfig(SettingGroup& resolvedGroup) {
	resolvedGroup.forEach([&](std::shared_ptr<Setting> set) {
		this->settings->forEach([&](std::shared_ptr<Setting> modSet) {
			if (modSet->name() == set->name()) {
				// FIXME: this might cause trouble with std::string and other types
				switch (set->type) {
				case Setting::Type::Bool:
					*modSet->value = std::get<BoolValue>(set->resolvedValue);
					break;
				case Setting::Type::Int:
					*modSet->value = std::get<IntValue>(set->resolvedValue);
					break;
				case Setting::Type::Float:
					*modSet->value = std::get<FloatValue>(set->resolvedValue);
					break;
				}
			}
			});
		});
}
