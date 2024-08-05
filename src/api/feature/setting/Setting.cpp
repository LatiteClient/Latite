#include "pch.h"
#include "Setting.h"
#include "SettingGroup.h"

// YES, this is the API folder, client specific stuff SHOULD NOT be in here at all, but
// i really dont want to go through everything that uses color1 and refactor manually...
#include <client/Latite.h>

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

StoredColor ColorValue::getMainColor() const {
    if (!isRGB) {
        return color1;
    }
    auto color1HSV = util::ColorToHSV({ color1.r, color1.g, color1.b, color1.a });
    color1HSV.h = Latite::get().getRGBHue()*360.f;
    auto col = util::HSVToColor(color1HSV);
    return { col.r, col.g, col.b, color1.a };
}
