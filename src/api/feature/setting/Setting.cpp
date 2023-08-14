#include "Setting.h"
#include "SettingGroup.h"

bool Setting::shouldRender(SettingGroup& group) {
    if (this->condition.type == Condition::NONE) return true;
    bool has = false;
    group.forEach([this, &has](std::shared_ptr<Setting> set) {
        if (has) return;
        if (set->name() != this->condition.settingName) return;
        std::visit([&](auto&& item) {
            int val = item.getInt();
            has = std::find(this->condition.values.begin(), this->condition.values.end(), val) != this->condition.values.end();
            }, *set->value);
        });
    return condition.type == Condition::EQUALS ? has : !has;
}
