#include "pch.h"
#include "SkinStealer.h"

#include "client/screen/ScreenManager.h"
#include "client/screen/screens/SkinStealerScreen.h"

SkinStealer::SkinStealer() : Module("SkinStealer", LocalizeString::get("client.module.skinStealer.name"),
                                    LocalizeString::get("client.module.skinStealer.desc"),
                                    GAME, nokeybind) {
}

void SkinStealer::onEnable() {
	Latite::getScreenManager().showScreen<SkinStealerScreen>();
}

void SkinStealer::onDisable() {
}

void SkinStealer::loadConfig(SettingGroup& resolvedGroup) {
	resolvedGroup.forEach([&](std::shared_ptr<Setting> set) {
		if (set->name() == "enabled" || set->name() == "key") return;

		this->settings->forEach([&](std::shared_ptr<Setting> modSet) {
			if (modSet->name() != set->name()) return;

			std::visit([&](auto&& obj) {
				*modSet->value = obj;
				modSet->update();
			}, set->resolvedValue);
		});
	});

	std::get<BoolValue>(enabled).value = false;
}
