#include "pch.h"
#include "ReachDisplay.h"
#include <mc/common/world/level/HitResult.h>

ReachDisplay::ReachDisplay() : TextModule("ReachDisplay", LocalizeString::get("client.textmodule.reachDisplay.name"),
                                          LocalizeString::get("client.textmodule.reachDisplay.desc"), HUD) {
	listen<AttackEvent>((EventListenerFunc)&ReachDisplay::onAttack);

	std::get<TextValue>(this->prefix) = std::wstring(L"Reach: ");
	std::get<TextValue>(this->suffix) = std::wstring(L" blocks");

	addSliderSetting("decimals", LocalizeString::get("client.textmodule.reachDisplay.decimals.name"),
                     LocalizeString::get("client.textmodule.reachDisplay.decimals.desc"), this->decimals,
                     FloatValue(0.f), FloatValue(6.f), FloatValue(1.f));
}

std::wstringstream ReachDisplay::text(bool isDefault, bool inEditor) {
	if (std::chrono::system_clock::now() - lastAttack > 3s) {
		reach = 0.0f;
	}

	return (std::wstringstream() << std::fixed << std::setprecision((size_t)(std::get<FloatValue>(decimals)))) << reach;
}

void ReachDisplay::onAttack(Event& evG) {
	auto level = SDK::ClientInstance::get()->minecraft->getLevel();
	auto hitResult = level->getHitResult(); // the exact point the player hit

	auto hitPoint = hitResult->hitPos;

	reach = hitResult->start.distance(hitPoint);
	lastAttack = std::chrono::system_clock::now();
}
