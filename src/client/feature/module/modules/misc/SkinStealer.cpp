#include "pch.h"
#include "SkinStealer.h"

#include "client/screen/ScreenManager.h"
#include "client/screen/screens/SkinStealerScreen.h"

SkinStealer::SkinStealer() : Module("SkinStealer", L"Skin Stealer",
                                    L"Opens a menu for saving player skins from the current player list.",
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
